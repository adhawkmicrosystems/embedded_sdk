#pragma once

#define AH_MIN(a, b) (((a) < (b)) ? (a) : (b))
#define AH_MAX(a, b) (((a) > (b)) ? (a) : (b))

#define AH_ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
