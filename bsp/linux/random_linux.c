#include "bsp_random.h"

#include <fcntl.h>
#include <string.h>
#include <unistd.h>

void ah_random(void *data, unsigned len)
{
    int fd = open("/dev/urandom", O_RDONLY);
    if (read(fd, data, len) < (int)len)
    {
        // I don't think this can actually happen, but it's here to suppress a compiler error
        memset(data, 0, len);
    }
    close(fd);
}
