#include "request.h"

#include "bsp_alloc.h"
#include "bsp_logging.h"
#include "bsp_thread.h"
#include "packet.h"
#include "spi_master.h"

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

struct request
{
    uint8_t type;           // Request type
    uint16_t requestId;     // request id used for API 2.0 requests
    ah_result result;       // Response result
    void **response;        // Response data (if requested)
    uint32_t *responseLen;  // Length of response data
    ah_sem_t *signal;       // Signal to wakeup the thread waiting for the response
    unsigned timeoutMs;     // Length of time to wait for the response
    bool fullResponse;      // If true include the type and ack code in the response data
};

static volatile int s_shuttingDown;
// Only 1 request can be in-flight at a time, since the API is currently blocking.
// At some point in the future this may be replaced with a queue to support async requests
static struct request *s_curRequest;

static ah_mutex_t *s_requestMutex;

static uint16_t s_nextRequestId;

static unsigned int request_get_timeout(uint8_t type)
{
    return type == ah_packetType_TriggerAutoTune ? 5000 : 1000;
}

static void freeRequest(struct request *request)
{
    ah_semaphore_destroy(request->signal);
    ah_free(request);
}

static ah_result waitForResponse(struct request *request)
{
    ah_result result;
    bool success = ah_semaphore_take(request->signal, request->timeoutMs);
    if (!success)
    {
        ah_log_warn("Request timeout: %#x", request->type);
        result = ah_result_RequestTimeout;
        if (request->response && request->fullResponse)
        {
            uint8_t ackData[2] = { request->type, ah_result_RequestTimeout };
            if (request->responseLen)
            {
                *request->responseLen = sizeof(ackData);
            }
            *request->response = ah_malloc(sizeof(ackData));
            memcpy(*request->response, ackData, sizeof(ackData));
        }
    }
    else
    {
        result = request->result;
    }
    return result;
}

void ah_request_handleResponse(const uint8_t *data, uint32_t len)
{
    struct request *request = s_curRequest;
    if (!request)
    {
        ah_log_error("Unexpected response: %#x", data[0]);
        return;
    }

    if (len < 3)
    {
        request->result = ah_result_UnexpectedResponse;
        ah_log_error("Unexpected response: Response too short! response len: %" PRIu32 "", len);
        return;
    }

    uint16_t id = 0;
    memcpy(&id, data, sizeof(id));

    if (id != request->requestId)
    {
        request->result = ah_result_UnexpectedResponse;
        ah_log_error("Unexpected response: RequestId=%#4x , ResponseId=%#4x", request->requestId, id);
        return;
    }
    data += 2;
    len -= 2;

    uint32_t responseLen;
    if (request->type == 0xa0)
    {
        request->result = ah_result_Success;
        responseLen = request->fullResponse ? len : len - 1;  // type
    }
    else
    {
        request->result = data[1];
        responseLen = request->fullResponse ? len : len - 2;  // type + result
    }

    if (request->responseLen)
    {
        *request->responseLen = responseLen;
    }

    if (request->response)
    {
        if (responseLen)
        {
            *request->response = ah_malloc(responseLen);
            if (*request->response)
            {
                memcpy(*request->response, &data[len - responseLen], responseLen);
            }
        }
        else
        {
            request->response = NULL;
        }
    }

    ah_semaphore_give(request->signal);
}

bool ah_request_waitingForResponse(uint16_t requestId)
{
    return s_curRequest && s_curRequest->requestId == requestId;
}

static ah_result sendRequest(const void *requestData, uint32_t requestLen,
                             void **response, uint32_t *responseLen, bool fullResponse)
{
    if (requestLen > API2_MAX_PACKET_SIZE - API2_PKT_OVERHEAD - sizeof(s_nextRequestId))
    {
        return ah_result_Failure;
    }

    if (!ah_mutex_lock(s_requestMutex, AH_NO_TIMEOUT))
    {
        ah_log_error("Unable to wait for request mutex");
        return ah_result_Failure;
    }

    const uint8_t *requestBytes = requestData;

    struct request *request = ah_malloc(sizeof(struct request));
    if (!request)
    {
        ah_log_error("Failed to send request: OOM");
        return ah_result_Failure;
    }

    *request = (struct request){
        .type = requestBytes[0],
        .requestId = s_nextRequestId++,
        .response = response,
        .responseLen = responseLen,
        .signal = ah_semaphore_create(0, 1),
        .timeoutMs = request_get_timeout(requestBytes[0]),
        .fullResponse = fullResponse,
    };
    if (!request->signal)
    {
        ah_log_error("Unable to create request signal");
    }
    s_curRequest = request;

    uint8_t buffer[API2_MAX_PACKET_SIZE];

    memcpy(buffer, &request->requestId, sizeof(request->requestId));
    memcpy(buffer + sizeof(request->requestId), requestBytes, requestLen);
    requestLen += sizeof(request->requestId);
    requestBytes = buffer;

    ah_result result;
    if (ah_spi_master_send(requestBytes, requestLen))
    {
        result = waitForResponse(s_curRequest);
    }
    else
    {
        ah_log_error("Failed to send request: tracker disconnected?");
        result = ah_result_CommunicationError;
    }

    freeRequest(request);
    s_curRequest = NULL;

    ah_mutex_unlock(s_requestMutex);
    if (s_shuttingDown)
    {
        if (response && *response)
        {
            ah_free(*response);
        }
        return ah_result_CommunicationError;
    }
    return result;
}

ah_result ah_request_sendRequest(const void *request, uint32_t requestLen,
                                 void **response, uint32_t *responseLen)
{
    return sendRequest(request, requestLen, response, responseLen, false);
}

void ah_request_forwardRequest(const uint8_t *request, uint32_t requestLen)
{
    ah_spi_master_send(request, requestLen);
}

void ah_request_sendRequestNoResponse(const void *request, uint32_t requestLen)
{
    if (requestLen > API2_MAX_PACKET_SIZE - API2_PKT_OVERHEAD - sizeof(s_nextRequestId))
    {
        return;
    }

    uint8_t buffer[API2_MAX_PACKET_SIZE];
    ah_mutex_lock(s_requestMutex, AH_NO_TIMEOUT);
    uint16_t id = s_nextRequestId++;
    ah_mutex_unlock(s_requestMutex);
    memcpy(buffer, &id, 2);
    memcpy(buffer + 2, request, requestLen);
    request = buffer;
    requestLen += 2;
    ah_spi_master_send(request, requestLen);
}

bool ah_request_init(void)
{
    if (s_requestMutex)
    {
        // Already initialized
        return true;
    }

    s_shuttingDown = 0;
    s_nextRequestId = 0;  // Could be random, but doesn't really matter

    s_requestMutex = ah_mutex_create();
    if (!s_requestMutex)
    {
        return false;
    }

    return true;
}

bool ah_request_deinit(void)
{
    s_shuttingDown = 1;

    if (s_requestMutex)
    {
        // The lock waits for `sendRequest` to complete
        ah_mutex_lock(s_requestMutex, AH_NO_TIMEOUT);
        ah_mutex_unlock(s_requestMutex);
        ah_mutex_destroy(s_requestMutex);
        s_requestMutex = NULL;
    }

    return true;
}
