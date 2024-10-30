#include "ah_sdk.h"

#include "ah_api.h"
#include "ah_api_decoders.h"
#include "ah_types.h"
#include "request.h"
#include "spi_master.h"
#include "tracker_info.h"
#include "util.h"

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static bool s_initialized;
static ah_trackerDataCallback s_trackerDataCallback;
static ah_trackerReadyCallback s_trackerReadyCallback;
static ah_eyeTrackingStreamsCallback s_etCallback;
static void *s_etCallbackCtx;

struct eventInfo
{
    const unsigned controlBit;
    ah_eventStreamCallback callback;
    void *ctx;
};

static struct eventInfo s_eventMap[] = {
    [ah_eventType_Blink] = {
        .controlBit = ah_eventControl_Blink,
    },
    [ah_eventType_EyeClosed] = {
        .controlBit = ah_eventControl_EyeOpenClose,
    },
    [ah_eventType_EyeOpened] = {
        .controlBit = ah_eventControl_EyeOpenClose,
    },
    [ah_eventType_Saccade] = {
        .controlBit = ah_eventControl_Saccade,
    },
    [ah_eventType_PerEyeSaccadeStart] = {
        .controlBit = ah_eventControl_PerEyeSaccadeStartEnd,
    },
    [ah_eventType_PerEyeSaccadeEnd] = {
        .controlBit = ah_eventControl_PerEyeSaccadeStartEnd,
    },
    [ah_eventType_TracklossStart] = {
        .controlBit = ah_eventControl_TracklossStartEnd,
    },
    [ah_eventType_TracklossEnd] = {
        .controlBit = ah_eventControl_TracklossStartEnd,
    },
    [ah_eventType_Fixation] = {
        .controlBit = ah_eventControl_Fixation,
    },
    [ah_eventType_FixationStart] = {
        .controlBit = ah_eventControl_Fixation,
    },
    [ah_eventType_ValidationSample] = {
        .controlBit = ah_eventControl_ValidationResults,
    },
    [ah_eventType_ValidationSummary] = {
        .controlBit = ah_eventControl_ValidationResults,
    },
    [ah_eventType_ProcedureStarted] = {
        .controlBit = ah_eventControl_ProcedureStartEnd,
    },
    [ah_eventType_ProcedureEnded] = {
        .controlBit = ah_eventControl_ProcedureStartEnd,
    },
    [ah_eventType_ExternalTrigger] = {
        .controlBit = ah_eventControl_ExternalTrigger,
    },
    [ah_eventType_GazeDepth] = {
        .controlBit = ah_eventControl_GazeDepth,
    },
};

static const float s_eyeTrackingRateMap[ah_eyeTrackingRate_Count] = {
    [ah_eyeTrackingRate_Off] = 0,
    [ah_eyeTrackingRate_5Hz] = 5,
    [ah_eyeTrackingRate_30Hz] = 30,
    [ah_eyeTrackingRate_60Hz] = 59,
    [ah_eyeTrackingRate_90Hz] = 90,
    [ah_eyeTrackingRate_125Hz] = 125,
    [ah_eyeTrackingRate_200Hz] = 200,
    [ah_eyeTrackingRate_250Hz] = 250,
    [ah_eyeTrackingRate_333Hz] = 333,
    [ah_eyeTrackingRate_500Hz] = 500,
};

static void handleEyeTrackingStream(const uint8_t *data, uint32_t len)
{
    ah_eyeTrackingStreamData etData = {};

    if (!s_etCallback)
    {
        return;
    }

    if (ah_api_decodeEyeTrackingStream(&etData, data, len) == ah_result_Success)
    {
        s_etCallback(&etData, s_etCallbackCtx);
    }
}

static void streamHandler(const uint8_t *data, uint32_t len)
{
    (void)len;

    if (data[0] == ah_packetType_EyeTrackingStream)
    {
        handleEyeTrackingStream(data, len);
        return;
    }

    if (data[0] == ah_packetType_TrackersReady)
    {
        if (s_trackerReadyCallback)
        {
            s_trackerReadyCallback();
        }
        return;
    }

    if (data[0] == ah_packetType_EventStream)
    {
        if (data[1] < AH_ARRAY_SIZE(s_eventMap) && s_eventMap[data[1]].callback)
        {
            s_eventMap[data[1]].callback((const ah_eventStreamData *)&data[1], s_eventMap[data[1]].ctx);
        }
        return;
    }
}

static void packetHandler(uint8_t isStream, const uint8_t *data, uint32_t len)
{
    if (!isStream)
    {
        uint16_t requestId = 0;
        memcpy(&requestId, data, sizeof(requestId));

        if (ah_request_waitingForResponse(requestId))
        {
            ah_request_handleResponse(data, len);
            return;
        }
    }
    else
    {
        streamHandler(data, len);
    }

    if (s_trackerDataCallback)
    {
        s_trackerDataCallback(isStream, data, len);
    }
}

AH_API bool ah_sdk_init(ah_trackerDataCallback trackerDataCb)
{
    if (s_initialized)
    {
        return false;
    }

    ah_request_init();
    ah_spi_master_init(packetHandler);

    // let tracker boot up
    unsigned retries = 5;
    while (!ah_tracker_info_init() && retries)
    {
        retries--;
    }

    if (!retries)
    {
        // tracker failed to connect
        return false;
    }

    s_trackerDataCallback = trackerDataCb;
    s_initialized = true;
    return true;
}

AH_API bool ah_sdk_deinit(void)
{
    ah_spi_master_deinit();
    ah_tracker_info_deinit();
    s_trackerReadyCallback = NULL;
    s_trackerDataCallback = NULL;
    s_initialized = false;
    return true;
}

AH_API void ah_sdk_forwardRequest(const uint8_t *data, uint32_t len)
{
    ah_request_forwardRequest(data, len);
}

AH_API const char *ah_sdk_resultToString(ah_result result)
{
    switch (result)
    {
        case ah_result_Success:
            return "Success";
        case ah_result_Failure:
            return "Internal failure";
        case ah_result_InvalidArgument:
            return "Invalid argument";
        case ah_result_TrackerNotReady:
            return "Tracker not ready";
        case ah_result_EyesNotFound:
            return "No eyes detected";
        case ah_result_RightEyeNotFound:
            return "Right eye not detected";
        case ah_result_LeftEyeNotFound:
            return "Left eye not detected";
        case ah_result_NotCalibrated:
            return "Gaze not calibrated";
        case ah_result_NotSupported:
            return "Not supported";
        case ah_result_SessionAlreadyRunning:
            return "Data logging session already exists";
        case ah_result_NoCurrentSession:
            return "No data logging session exists to stop";
        case ah_result_RequestTimeout:
            return "Request has timed out";
        case ah_result_UnexpectedResponse:
            return "Unexpected response";
        case ah_result_HardwareFault:
            return "Hardware faulted";
        case ah_result_CameraFault:
            return "Camera initialization failed";
        case ah_result_Busy:
            return "System is busy";
        case ah_result_CommunicationError:
            return "Communication error";
        case ah_result_DeviceCalibrationRequired:
            return "Device calibration is outdated";
        case ah_result_ProcessIncomplete:
            return "Process was aborted or interrupted unexpectedly";
        case ah_result_InactiveInterface:
            return "Packet received from an inactive interface";
        default:
            return "Unknown error";
    }
}

AH_API ah_result ah_sdk_enableTracking(bool enable)
{
    return ah_api_enableTracking(enable);
}

AH_API ah_result ah_sdk_registerTrackerReady(ah_trackerReadyCallback trackerReadyCb)
{
    s_trackerReadyCallback = trackerReadyCb;
    return ah_result_Success;
}

AH_API ah_result ah_sdk_getTrackerStatus(void)
{
    return ah_api_getTrackerStatus();
}

AH_API ah_result ah_sdk_setEyeTrackingDataCallback(ah_eyeTrackingStreamsCallback callback, void *ctx)
{
    s_etCallback = callback;
    s_etCallbackCtx = ctx;
    return ah_result_Success;
}

AH_API ah_result ah_sdk_eyeTrackingStreamControl(uint32_t mask, bool enable)
{
    return ah_api_setEyeTrackingStreams(mask, enable);
}

AH_API ah_result ah_sdk_getEyeTrackingStreams(uint32_t *mask)
{
    if (!mask)
    {
        return ah_result_InvalidArgument;
    }

    return ah_api_getEyeTrackingStreams(mask);
}

AH_API ah_result ah_sdk_setEyeTrackingRate(ah_eyeTrackingRate eyeTrackingRate)
{
    if (eyeTrackingRate >= ah_eyeTrackingRate_Count || eyeTrackingRate < 0)
    {
        return ah_result_InvalidArgument;
    }
    return ah_api_setEyeTrackingStreamRate(s_eyeTrackingRateMap[eyeTrackingRate]);
}

AH_API ah_result ah_sdk_getEyeTrackingRate(ah_eyeTrackingRate *eyeTrackingRate)
{
    if (!eyeTrackingRate)
    {
        return ah_result_InvalidArgument;
    }
    float rate = 0;
    ah_result rv = ah_api_getEyeTrackingStreamRate(&rate);

    if (rv == ah_result_Success)
    {
        float diff;
        for (ah_eyeTrackingRate i = 0; i < ah_eyeTrackingRate_Count; i++)
        {
            // fabs isn't available on all platforms
            diff = s_eyeTrackingRateMap[i] < rate ? rate - s_eyeTrackingRateMap[i] : s_eyeTrackingRateMap[i] - rate;

            // these won't exactly match as tracker uses
            // ms periods so 60Hz = 17ms period. But 17ms period = 59Hz
            // so check for roughly equal
            if (diff < 1.2f)
            {
                *eyeTrackingRate = i;
                break;
            }
        }
    }
    return rv;
}

AH_API ah_result ah_sdk_setupEvent(ah_eventControl eventControl, bool enable, ah_eventStreamCallback eventCb, void *ctx)
{
    for (unsigned i = 0; i < AH_ARRAY_SIZE(s_eventMap); i++)
    {
        if (s_eventMap[i].controlBit == eventControl)
        {
            s_eventMap[i].callback = eventCb;
            s_eventMap[i].ctx = ctx;
        }
    }

    return ah_api_setEventControl(eventControl, enable);
}

AH_API ah_result ah_sdk_runAutotune(ah_autotuneReferenceGazeVector *referenceGazeVector)
{
    return ah_api_triggerAutotune(referenceGazeVector);
}

AH_API ah_result ah_sdk_calibrationStart(ah_calibrationMode mode)
{
    return ah_api_calibrationStart(mode);
}

AH_API ah_result ah_sdk_calibrationComplete(void)
{
    return ah_api_calibrationComplete();
}

AH_API ah_result ah_sdk_calibrationAbort(void)
{
    return ah_api_calibrationAbort();
}

AH_API ah_result ah_sdk_calibrationRegisterSample(unsigned idx)
{
    return ah_api_calibrationRegisterSample(idx);
}

AH_API ah_result ah_sdk_calibrationRegisterPoint(unsigned idx, float xPos, float yPos, float zPos)
{
    return ah_api_calibrationRegisterPoint(idx, xPos, yPos, zPos);
}

AH_API ah_result ah_sdk_calibrationRecenter(float xPos, float yPos, float zPos)
{
    return ah_api_calibrationRecenter(xPos, yPos, zPos);
}

AH_API ah_result ah_sdk_validationStart(void)
{
    return ah_api_validationStart();
}

AH_API ah_result ah_sdk_validationComplete(void)
{
    return ah_api_validationComplete();
}

AH_API ah_result ah_sdk_validationRegisterPoint(float xPos, float yPos, float zPos)
{
    return ah_api_validationRegisterPoint(xPos, yPos, zPos);
}

AH_API ah_result ah_sdk_getNominalEyeOffsets(float offsets[AH_NUM_EYES][AH_XYZ_SIZE])
{
    return ah_api_getNominalEyeOffsets(offsets);
}

AH_API ah_result ah_sdk_setDataloggingMode(ah_logMode mode)
{
    return ah_api_setDataloggingMode(mode);
}

AH_API unsigned ah_sdk_trackerInfo_getEyeMask(void)
{
    return ah_tracker_info_getEyeMask();
}

AH_API unsigned ah_sdk_trackerInfo_getProductId(void)
{
    return ah_tracker_info_getProductId();
}

AH_API char *ah_sdk_trackerInfo_getSerialNumber(void)
{
    return ah_tracker_info_getSerialNumber();
}

AH_API char *ah_sdk_trackerInfo_getFirmwareVersion(void)
{
    return ah_tracker_info_getFirmwareVersion();
}

AH_API char *ah_sdk_trackerInfo_getAPIVersion(void)
{
    return ah_tracker_info_getAPIVersion();
}

AH_API bool ah_sdk_trackerInfo_isUpdater(void)
{
    return ah_tracker_info_isUpdater();
}
