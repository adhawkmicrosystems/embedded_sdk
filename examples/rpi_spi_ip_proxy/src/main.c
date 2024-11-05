#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ah_sdk.h"
#include "packet.h"      // Included to help add / remove packet headers
#include "spi_master.h"  // Included to increase SPI polling rate to increase throughput

static int s_clientfd;
static struct sockaddr_in s_clientaddr;

static void socketSend(const uint8_t *data, uint32_t len)
{
    if (s_clientfd)
    {
        send(s_clientfd, data, len, 0);
    }
}

#ifndef ECHO_SERVER
static void trackerPacketHandler(uint8_t isStream, const uint8_t *data, uint32_t len)
{
    (void)isStream;

    uint8_t packet[API2_MAX_PACKET_SIZE];
    packet[0] = API2_PKT_SOP;
    uint16_t header = (len & API2_LENGTH_MASK);
    if (isStream)
    {
        header |= API2_STREAM_MASK;
    }
    memcpy(packet + 1, &header, sizeof(header));
    memcpy(packet + API2_PKT_OVERHEAD, data, len);

    socketSend(packet, API2_PKT_OVERHEAD + len);
}
#endif

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Please specify a TCP port\n");
        return 1;
    }
    const int port = atoi(argv[1]);

    int serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverfd < 0)
    {
        printf("Failed to create socket\n");
        return 1;
    }

    int enable = 1;
    setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (const void *)&enable, sizeof(enable));

    struct sockaddr_in server_address = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = htonl(INADDR_ANY),
        .sin_port = htons(port),
    };

    if (bind(serverfd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        printf("Failed to bind\n");
        return 1;
    }

    if (listen(serverfd, 1) < 0)
    {
        printf("Failed to listen\n");
        return 1;
    }

#ifndef ECHO_SERVER
    if (!ah_sdk_init(trackerPacketHandler))
    {
        printf("Tracker failed to connect\n");
        return 1;
    }
    else
    {
        printf("Tracker successfully connected!\n");
        printf("Tracker info: %s: %s v%s\n", ah_sdk_trackerInfo_getSerialNumber(),
               ah_sdk_trackerInfo_getFirmwareVersion(),
               ah_sdk_trackerInfo_getAPIVersion());
    }
#endif
    // Improve throughput when proxying pulses and fwup data
    ah_spi_master_setRate(ah_spiPollingRate_Fast);

    int len = 0;
    uint8_t data[1024];

    while (true)
    {
        socklen_t clientlen = sizeof(s_clientaddr);
        s_clientfd = accept(serverfd, (struct sockaddr *)&s_clientaddr, &clientlen);
        if (s_clientfd < 0)
        {
            printf("Failed to accept\n");
            return 1;
        }

        while (true)
        {
            int recvLen = recv(s_clientfd, data + len, sizeof(data) - len, 0);
            if (recvLen <= 0)
            {
                // Client disconnected
                break;
            }
            len += recvLen;

#ifndef ECHO_SERVER
            // TCP data frames can contain multiple or partial packets
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
                    memmove(data, p, len);
                    break;
                }

                ah_sdk_forwardRequest(p + API2_PKT_OVERHEAD, data_len);
                p += packet_len;
                len -= packet_len;
            }

            if (len)
            {
                memmove(data, p, len);
            }
#else
            socketSend(data, len);
#endif
        }
        s_clientfd = 0;
    }

    return 0;
}
