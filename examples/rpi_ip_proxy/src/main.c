#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ah_sdk.h"

static int s_socketfd;
static struct sockaddr_in s_clientaddr;

static void socketSend(const uint8_t *data, uint32_t len)
{
    sendto(s_socketfd, data, len, 0, (struct sockaddr *)&s_clientaddr, sizeof(s_clientaddr));
}

static void trackerPacketHandler(uint8_t isStream, const uint8_t *data, uint32_t len)
{
    (void)isStream;
    socketSend(data, len);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Please specify a port\n");
        return 1;
    }
    const int port = atoi(argv[1]);

    s_socketfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (s_socketfd < 0)
    {
        printf("Failed to create socket\n");
        return 1;
    }

    int enable = 1;
    setsockopt(s_socketfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&enable, sizeof(enable));

    struct sockaddr_in server_address = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = htonl(INADDR_ANY),
        .sin_port = htons(port),
    };

    if (bind(s_socketfd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        printf("Failed to bind\n");
        return 1;
    }

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

    socklen_t clientlen = sizeof(s_clientaddr);
    uint8_t data[1024];
    while (true)
    {
        int len = recvfrom(s_socketfd, data, sizeof(data), 0, (struct sockaddr *)&s_clientaddr, &clientlen);

        // Acking these commands allows reuse of some host side code
        if (data[2] == ah_packetType_UDPConnection ||
            data[2] == ah_packetType_UDPDisconnect ||
            data[2] == ah_packetType_UDPPing)
        {
            const char ack[] = { data[0], data[1], data[2], ah_result_Success };
            socketSend(ack, sizeof(ack));
            continue;
        }

        ah_sdk_forwardRequest(data, len);
    }

    return 0;
}
