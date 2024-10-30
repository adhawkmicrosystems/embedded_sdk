#pragma once

#include "ah_types.h"

#include <stdbool.h>
#include <stdint.h>

// Publically documented API functions

ah_result ah_api_enableTracking(bool enable);

ah_result ah_api_getTrackerStatus(void);
//! result will contain a sequence of <type>, <size>, <data> for each type requested
//! The result pointer must be free'd
ah_result ah_api_getTrackerInfo(ah_trackerInfo types[], unsigned numTypes, uint8_t **result, unsigned *retultLen);

//! Bitmask may contain multiple streams
ah_result ah_api_setStreamControl(unsigned streamControlBitmask, float rateHz);
ah_result ah_api_getStreamControl(unsigned streamControlBitmask, float *rateHz);

//! Bitmask for enable eyetracking streams
ah_result ah_api_setEyeTrackingStreams(uint32_t eyeTrackingStreamMask, bool enable);

ah_result ah_api_getEyeTrackingStreams(uint32_t *eyeTrackingStreamMask);

ah_result ah_api_setFeatureStreams(uint32_t featureStreamMask, bool enable);

ah_result ah_api_getFeatureStreams(uint32_t *featureStreamMask);

ah_result ah_api_setEyeTrackingStreamRate(float rateHz);

ah_result ah_api_getEyeTrackingStreamRate(float *rateHz);

//! Bitmask may contain multiple streams
ah_result ah_api_setEventControl(unsigned eventControlBitmask, bool enable);
ah_result ah_api_getEventControl(unsigned *enabledEventBitmask);

//! This can take several seconds to return
ah_result ah_api_triggerAutotune(ah_autotuneReferenceGazeVector *referenceGazeVector);

ah_result ah_api_calibrationStart(ah_calibrationMode mode);
ah_result ah_api_calibrationComplete(void);
ah_result ah_api_calibrationAbort(void);
ah_result ah_api_calibrationRegisterSample(unsigned idx);
ah_result ah_api_calibrationRegisterPoint(unsigned idx, float xPos, float yPos, float zPos);
ah_result ah_api_calibrationRecenter(float xPos, float yPos, float zPos);

ah_result ah_api_validationStart(void);
ah_result ah_api_validationComplete(void);
ah_result ah_api_validationRegisterPoint(float xPos, float yPos, float zPos);

//! Offset data is in mm
ah_result ah_api_getNominalEyeOffsets(float offsets[AH_NUM_EYES][AH_XYZ_SIZE]);

//! Sets the datalogging mode
ah_result ah_api_setDataloggingMode(ah_logMode mode);
