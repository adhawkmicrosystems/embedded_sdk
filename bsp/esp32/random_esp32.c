#include "bsp_random.h"

#include "esp_random.h"

void ah_random(void *data, unsigned len)
{
    esp_fill_random(data, len);
}
