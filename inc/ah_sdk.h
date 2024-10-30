#pragma once

#include "ah_types.h"
#include "ah_typesEyetracking.h"

#include <stdbool.h>
#include <stdint.h>

#define AH_API __attribute__((visibility("default")))

typedef enum
{
    ah_eyeTrackingRate_Off,
    ah_eyeTrackingRate_5Hz,
    ah_eyeTrackingRate_30Hz,
    ah_eyeTrackingRate_60Hz,
    ah_eyeTrackingRate_90Hz,
    ah_eyeTrackingRate_125Hz,
    ah_eyeTrackingRate_200Hz,
    ah_eyeTrackingRate_250Hz,
    ah_eyeTrackingRate_333Hz,
    ah_eyeTrackingRate_500Hz,

    ah_eyeTrackingRate_Count,
} ah_eyeTrackingRate;

//! All eye tracking stream data in one place
//! Not all contents are always valid, check ah_eyeTrackingStreamData::validMask
//! This callback will run on a different thread
//! This callback can't make any API calls
//! The runtime of this callback should be minimized to avoid dropping/blocking responses or other stream callbacks
typedef void (*ah_eyeTrackingStreamsCallback)(const ah_eyeTrackingStreamData *streamData, void *ctx);

//! Callback for event stream data
//! This callback will run on a different thread
//! This callback can't make any API calls
//! The runtime of this callback should be minimized to avoid dropping/blocking responses or other stream callbacks
typedef void (*ah_eventStreamCallback)(const ah_eventStreamData *eventData, void *ctx);

//! Callback for the tracker ready message
//! This callback will run on a different thread
//! This callback can't make any API calls
//! The runtime of this callback should be minimized to avoid dropping/blocking responses or other stream callbacks
typedef void (*ah_trackerReadyCallback)(void);

//! Callback for all data which is not a request generated by the SDK
//! This is useful when the SDK is used to proxy requests from an external source, or when handling types not supported by the SDK
//! This callback will run on a different thread
//! This callback can't make any API calls
//! The runtime of this callback should be minimized to avoid dropping/blocking responses or other stream callbacks
typedef void (*ah_trackerDataCallback)(uint8_t isStream, const uint8_t *data, uint32_t len);

//! Initialize the SDK and connects to the tracker. Must be called before anything else
//! trackerDataCb is optional if not using the `ah_sdk_forwardRequest` API
AH_API bool ah_sdk_init(ah_trackerDataCallback trackerDataCb);

//! De-initialize the API
AH_API bool ah_sdk_deinit(void);

//! Sends a fully formed request from an external source to the tracker
//! The response will be available in the `ah_trackerDataCallback`
AH_API void ah_sdk_forwardRequest(const uint8_t *data, uint32_t len);

//! Return a user friendly string for a given result code
AH_API const char *ah_sdk_resultToString(ah_result result);

//! Enables or disables eye tracking
AH_API ah_result ah_sdk_enableTracking(bool enable);

//! Registers a callback for tracker ready messages
//! Only one callback is supported. Repeated calls will replace the previous callback
AH_API ah_result ah_sdk_registerTrackerReady(ah_trackerReadyCallback trackerReadyCb);

//! Queries the status of the tracker
AH_API ah_result ah_sdk_getTrackerStatus(void);

//! Sets the callback for eye tracking data
//! Only one callback per stream type is supported
//! Repeated calls will replace the callback for the given type
AH_API ah_result ah_sdk_setEyeTrackingDataCallback(ah_eyeTrackingStreamsCallback callback, void *ctx);

//! Enable or disable the eye tracking stream types in the mask
AH_API ah_result ah_sdk_eyeTrackingStreamControl(uint32_t mask, bool enable);

//! Get the enabled eye tracking streams
AH_API ah_result ah_sdk_getEyeTrackingStreams(uint32_t *mask);

//! Sets the rate of the eyetracking stream
AH_API ah_result ah_sdk_setEyeTrackingRate(ah_eyeTrackingRate eyeTrackingRate);

//! Gets the current rate of the eyetracking stream
AH_API ah_result ah_sdk_getEyeTrackingRate(ah_eyeTrackingRate *eyeTrackingRate);

//! Sets the specified event control bit
AH_API ah_result ah_sdk_setupEvent(ah_eventControl eventControl, bool enable, ah_eventStreamCallback eventCb, void *ctx);

//! Runs an autotune, this can take several seconds to return
//! referenceGazeVec is optional. If NULL the algorithm assumes that user is looking straight ahead
AH_API ah_result ah_sdk_runAutotune(ah_autotuneReferenceGazeVector *referenceGazeVec);

//! Begin a calibration
AH_API ah_result ah_sdk_calibrationStart(ah_calibrationMode mode);

//! Finish a calibration
AH_API ah_result ah_sdk_calibrationComplete(void);

//! Abandon a calibration
AH_API ah_result ah_sdk_calibrationAbort(void);

//! Add a calibration sample to an ongoing calibration
AH_API ah_result ah_sdk_calibrationRegisterSample(unsigned idx);

//! Add a calibration point to an ongoing calibration
AH_API ah_result ah_sdk_calibrationRegisterPoint(unsigned idx, float xPos, float yPos, float zPos);

//! Updates an existing calibration based on the specified point
AH_API ah_result ah_sdk_calibrationRecenter(float xPos, float yPos, float zPos);

//! Begin a validation
AH_API ah_result ah_sdk_validationStart(void);

//! Finish a validation
AH_API ah_result ah_sdk_validationComplete(void);

//! Add a validation point to an ongoing validation
AH_API ah_result ah_sdk_validationRegisterPoint(float xPos, float yPos, float zPos);

//! Read eye offsets. Offset data is in mm
AH_API ah_result ah_sdk_getNominalEyeOffsets(float offsets[AH_NUM_EYES][AH_XYZ_SIZE]);

//! Set the diagnostic log mode for the tracker
AH_API ah_result ah_sdk_setDataloggingMode(ah_logMode mode);

//! Friendlier tracker info calls for individual queries
AH_API unsigned ah_sdk_trackerInfo_getEyeMask(void);
AH_API unsigned ah_sdk_trackerInfo_getProductId(void);
AH_API char *ah_sdk_trackerInfo_getSerialNumber(void);
AH_API char *ah_sdk_trackerInfo_getFirmwareVersion(void);
AH_API char *ah_sdk_trackerInfo_getAPIVersion(void);
AH_API bool ah_sdk_trackerInfo_isUpdater(void);
