#pragma once

#include "ah_types.h"
#include "ah_typesEyetracking.h"

#include <stdint.h>

ah_result ah_api_decodeEyeTrackingStream(ah_eyeTrackingStreamData *stream, const uint8_t *data, uint32_t len);
