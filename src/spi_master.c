#include "spi_master.h"

#include "bsp_logging.h"
#include "bsp_spi.h"
#include "bsp_thread.h"
#include "bsp_timer.h"
#include "bsp_uptime.h"
#include "packet.h"

#include <string.h>

#define SPI_THREAD_STACK_SIZE 4096

// One buffer is used for sending while the others are filled
// FWUP send 3.5k at a time which requires 4 frames to buffer, plus 1 to send and 1 extra
#define SPI_TX_BUFFER_COUNT (6)

// SPI data rates are high when forwarding over USB (raw pulses / FWUP)
#define SPI_POLL_INTERVAL_FAST_US (1000)  // 1000 HZ
// SPI data rates are low when sending via BT only (events / low rate gaze)
#define SPI_POLL_INTERVAL_SLOW_US (5000)  // 200 HZ

// If we poll quicker than this the ST might not have time to process data we send
// The 27.5MHz clock + 1024 byte frame means each transaction will take at least:
// 0.036us * 1024 * 8 = 294.6us + some buffer copies = ~0.4ms
#define MIN_POLL_INTERVAL_US (400)

typedef struct __attribute__((__packed__))
{
    uint8_t SOF;  // `AH_SPI_SOF_BYTE` if valid
    uint16_t len;
    uint8_t data[AH_SPI_FRAME_SIZE - AH_SPI_HEADER_SIZE];
} spiFrame;
_Static_assert(sizeof(spiFrame) == AH_SPI_FRAME_SIZE, "Invalid SPI frame size");

static ah_thread_t *s_spiThread;

static volatile bool s_shuttingDown;

static ah_spiDataReadyCallback s_dataReadyCallback;

static ah_timer_t *s_pollTimer;
static ah_sem_t *s_pollSem;

static ah_mutex_t *s_txMutex;
static unsigned s_fillFrameIdx;
static unsigned s_sendFrameIdx;
static spiFrame s_txBuffer[SPI_TX_BUFFER_COUNT];

static void handleRxComplete(const uint8_t *data, uint32_t len)
{
    // SPI data frames can contain multiple packets
    const uint8_t *p = data;
    while (*p == API2_PKT_SOP && len > API2_PKT_OVERHEAD)
    {
        uint16_t header;
        memcpy(&header, p + 1, sizeof(header));
        const uint16_t data_len = header & API2_LENGTH_MASK;
        const uint16_t packet_len = data_len + API2_PKT_OVERHEAD;
        const bool isStream = header & API2_STREAM_MASK;

        if (len < packet_len)
        {
            // We don't send partial packets over SPI
            ah_log_warn("Missing SPI frame data");
            return;
        }
        if (packet_len > API2_MAX_PACKET_SIZE)
        {
            ah_log_warn("Invalid packet length");
            return;
        }

        s_dataReadyCallback(isStream, p + API2_PKT_OVERHEAD, data_len);
        p += packet_len;
        len -= packet_len;
    }

    if (len)
    {
        ah_log_warn("Unparsed SPI frame data");
    }
}

static void pollTimerCb(void)
{
    ah_semaphore_give(s_pollSem);
}

static void spi_thread(void)
{
    static int64_t s_lastPollTime;

    while (!s_shuttingDown)
    {
        ah_semaphore_take(s_pollSem, AH_NO_TIMEOUT);

        const int64_t now = ah_getUptimeUs();
        if (now - s_lastPollTime < MIN_POLL_INTERVAL_US)
        {
            // If the last poll was delayed, the next one might happen before the ET has a chance
            // to finishing the transaction. Enforce a minimum time between polls
            continue;
        }

        ah_mutex_lock(s_txMutex, AH_NO_TIMEOUT);
        s_sendFrameIdx = (s_sendFrameIdx + 1) % SPI_TX_BUFFER_COUNT;
        if (s_sendFrameIdx == s_fillFrameIdx)
        {
            s_fillFrameIdx = (s_fillFrameIdx + 1) % SPI_TX_BUFFER_COUNT;
        }
        ah_mutex_unlock(s_txMutex);

        spiFrame *writeFrame = &s_txBuffer[s_sendFrameIdx];
        writeFrame->SOF = writeFrame->len ? AH_SPI_SOF_BYTE : 0;

        spiFrame readFrame;
        memset(&readFrame, 0, AH_SPI_HEADER_SIZE);  // Zero out SOF / len

        ah_spi_transferFullDuplex((uint8_t *)writeFrame, (uint8_t *)&readFrame);
        s_lastPollTime = ah_getUptimeUs();

        if (writeFrame->len)
        {
            // Clear write buffer so it can be re-used
            memset(writeFrame, 0, AH_SPI_HEADER_SIZE);
        }

        if (readFrame.SOF == AH_SPI_SOF_BYTE && readFrame.len && readFrame.len <= AH_SPI_FRAME_SIZE)
        {
            handleRxComplete(readFrame.data, readFrame.len);
        }
    }
}

void ah_spi_master_init(ah_spiDataReadyCallback cb)
{
    if (!ah_spi_init())
    {
        return;
    }

    s_dataReadyCallback = cb;

    s_pollTimer = ah_timer_create(pollTimerCb);
    s_txMutex = ah_mutex_create();
    s_pollSem = ah_semaphore_create(0, 1);

    s_sendFrameIdx = 0;
    s_fillFrameIdx = 1;

    s_shuttingDown = false;
    s_spiThread = ah_thread_create(spi_thread, SPI_THREAD_STACK_SIZE, ah_spi_getThreadPriority(), "SPI RX");
    if (!s_spiThread)
    {
        ah_log_error("Failed to create SPI thread");
    }

    ah_timer_startPeriodic(s_pollTimer, SPI_POLL_INTERVAL_SLOW_US);
}

void ah_spi_master_deinit(void)
{
    s_shuttingDown = true;

    ah_timer_destroy(s_pollTimer);

    ah_semaphore_give(s_pollSem);
    ah_semaphore_destroy(s_pollSem);

    ah_mutex_destroy(s_txMutex);

    ah_thread_quit(s_spiThread);
    ah_thread_destroy(s_spiThread);

    ah_spi_deinit();
}

void ah_spi_master_setRate(ah_spiPollingRate rate)
{
    uint64_t interval;
    if (rate == ah_spiPollingRate_Slow)
    {
        interval = SPI_POLL_INTERVAL_SLOW_US;
    }
    else if (rate == ah_spiPollingRate_Fast)
    {
        interval = SPI_POLL_INTERVAL_FAST_US;
    }
    else
    {
        ah_log_error("Invalid poll rate");
        return;
    }

    ah_timer_startPeriodic(s_pollTimer, interval);
}

bool ah_spi_master_send(const uint8_t *data, uint32_t len)
{
    ah_mutex_lock(s_txMutex, AH_NO_TIMEOUT);

    const unsigned encodedLen = len + API2_PKT_OVERHEAD;

    spiFrame *fillFrame = &s_txBuffer[s_fillFrameIdx];
    if (fillFrame->len + encodedLen > sizeof(fillFrame->data))
    {
        const unsigned nextFillFrameIdx = (s_fillFrameIdx + 1) % SPI_TX_BUFFER_COUNT;
        if (nextFillFrameIdx == s_sendFrameIdx)
        {
            ah_mutex_unlock(s_txMutex);
            ah_log_error("SPI send buffers full");
            return false;
        }
        s_fillFrameIdx = nextFillFrameIdx;
        fillFrame = &s_txBuffer[s_fillFrameIdx];
    }

    uint8_t *fillPointer = fillFrame->data + fillFrame->len;

    uint16_t header = len & API2_LENGTH_MASK;
    fillPointer[0] = API2_PKT_SOP;
    memcpy(fillPointer + 1, &header, sizeof(header));
    memcpy(fillPointer + API2_PKT_OVERHEAD, data, len);
    fillFrame->len += encodedLen;
    ah_mutex_unlock(s_txMutex);

    return true;
}
