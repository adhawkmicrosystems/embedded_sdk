#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ah_sdk.h"

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

    uint8_t packet[128];
    packet[0] = 0xAA;
    uint16_t header = (len & 0x3FF);
    if (isStream)
    {
        header |= 0x8000;
    }
    memcpy(packet + 1, &header, sizeof(header));
    memcpy(packet + 3, data, len);

    socketSend(packet, 3 + len);
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
            int len = recv(s_clientfd, data, sizeof(data), 0);
            if (len <= 0)
            {
                // Client disconnected
                break;
            }

#ifndef ECHO_SERVER
            // Strip the header, it will get added back on
            ah_sdk_forwardRequest(data + 3, len - 3);
#else
            socketSend(data, len);
#endif
        }
        s_clientfd = 0;
    }

    return 0;
}
