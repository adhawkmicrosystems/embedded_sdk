#include "ah_api.h"

#include "ah_types.h"
#include "ah_typesEyetracking.h"
#include "bsp_alloc.h"
#include "request.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

ah_result ah_api_enableTracking(bool enable)
{
    struct __attribute__((__packed__))
    {
        uint8_t type;
        uint8_t subtype;
        uint8_t value;
    } request = {
        .type = ah_packetType_SystemControl,
        .subtype = ah_systemControlType_EyeTracking,
        .value = enable,
    };

    return ah_request_sendRequest(&request, sizeof(request), NULL, NULL);
}

ah_result ah_api_getTrackerStatus(void)
{
    struct __attribute__((__packed__))
    {
        uint8_t type;
    } request = {
        .type = ah_packetType_GetTrackerStatus,
    };

    return ah_request_sendRequest(&request, sizeof(request), NULL, NULL);
}

ah_result ah_api_getTrackerInfo(ah_trackerInfo types[], unsigned numTypes, uint8_t **result, unsigned *resultLen)
{
    unsigned requestLen = numTypes + 2 /* packet type, multi flag */;
    uint8_t *request = ah_malloc(requestLen);
    request[0] = ah_packetType_DeviceInfo;
    request[1] = ah_trackerInfo_Multi;

    for (unsigned i = 0; i < numTypes; i++)
    {
        request[i + 2] = types[i];
    }

    void *responseData = NULL;
    uint32_t responseLen = 0;
    ah_result rv = ah_request_sendRequest(request, requestLen, &responseData, &responseLen);
    ah_free(request);

    if (rv != ah_result_Success)
    {
        ah_free(responseData);
        return rv;
    }
    *result = responseData;
    *resultLen = responseLen;

    return rv;
}

ah_result ah_api_setStreamControl(unsigned streamControlBitmask, float rateHz)
{
    struct __attribute__((__packed__))
    {
        uint8_t type;
        uint8_t subtype;
        uint32_t enableBitfield;
        float dataRateHz;
    } request = {
        .type = ah_packetType_SetProperty,
        .subtype = ah_propertyType_StreamControl,
        .enableBitfield = streamControlBitmask,
        .dataRateHz = rateHz,
    };

    return ah_request_sendRequest(&request, sizeof(request), NULL, NULL);
}

ah_result ah_api_getStreamControl(unsigned streamControlBitmask, float *rateHz)
{
    struct __attribute__((__packed__))
    {
        uint8_t type;
        uint8_t subtype;
        uint32_t streamBitfield;
    } request = {
        .type = ah_packetType_GetProperty,
        .subtype = ah_propertyType_StreamControl,
        .streamBitfield = streamControlBitmask,
    };
    struct __attribute__((__packed__)) responseData
    {
        uint8_t subtype;
        float rate;
    };

    void *responsePtr = NULL;
    ah_result rv = ah_request_sendRequest(&request, sizeof(request), &responsePtr, NULL);
    if (rv == ah_result_Success)
    {
        struct responseData *response = responsePtr;
        memcpy(rateHz, &response->rate, sizeof(*rateHz));
    }
    ah_free(responsePtr);
    return rv;
}

ah_result ah_api_setEyeTrackingStreams(uint32_t eyeTrackingStreamMask, bool enable)
{
    struct __attribute__((__packed__))
    {
        uint8_t type;
        uint8_t subtype;
        uint32_t eyeTrackingMask;
        uint8_t enable;
    } request = {
        .type = ah_packetType_SetProperty,
        .subtype = ah_propertyType_EyeTrackingStreams,
        .eyeTrackingMask = eyeTrackingStreamMask,
        .enable = enable,
    };

    return ah_request_sendRequest(&request, sizeof(request), NULL, NULL);
}

ah_result ah_api_getEyeTrackingStreams(uint32_t *eyeTrackingStreamMask)
{
    struct __attribute__((__packed__))
    {
        uint8_t type;
        uint8_t subtype;
    } request = {
        .type = ah_packetType_GetProperty,
        .subtype = ah_propertyType_EyeTrackingStreams,
    };
    struct __attribute__((__packed__)) responseData
    {
        uint8_t subtype;
        uint32_t mask;
    };

    void *response = NULL;
    uint32_t len = 0;
    ah_result rv = ah_request_sendRequest(&request, sizeof(request), &response, &len);
    *eyeTrackingStreamMask = 0;
    if (rv == ah_result_Success)
    {
        struct responseData *data = response;
        memcpy(eyeTrackingStreamMask, &data->mask, sizeof(*eyeTrackingStreamMask));
    }
    ah_free(response);
    return rv;
}

ah_result ah_api_setFeatureStreams(uint32_t featureStreamMask, bool enable)
{
    struct __attribute__((__packed__))
    {
        uint8_t type;
        uint8_t subtype;
        uint32_t eyeTrackingMask;
        uint8_t enable;
    } request = {
        .type = ah_packetType_SetProperty,
        .subtype = ah_propertyType_FeatureStreams,
        .eyeTrackingMask = featureStreamMask,
        .enable = enable
    };

    return ah_request_sendRequest(&request, sizeof(request), NULL, NULL);
}

ah_result ah_api_getFeatureStreams(uint32_t *featureStreamMask)
{
    struct __attribute__((__packed__))
    {
        uint8_t type;
        uint8_t subtype;
    } request = {
        .type = ah_packetType_GetProperty,
        .subtype = ah_propertyType_FeatureStreams,
    };
    struct __attribute__((__packed__)) responseData
    {
        uint8_t subtype;
        uint32_t mask;
    };

    void *response = NULL;
    uint32_t len = 0;
    ah_result rv = ah_request_sendRequest(&request, sizeof(request), &response, &len);
    *featureStreamMask = 0;
    if (rv == ah_result_Success)
    {
        struct responseData *data = response;
        memcpy(featureStreamMask, &data->mask, sizeof(*featureStreamMask));
    }
    ah_free(response);
    return rv;
}

ah_result ah_api_setEyeTrackingStreamRate(float rateHz)
{
    struct __attribute__((__packed__))
    {
        uint8_t type;
        uint8_t subtype;
        float rate;
    } request = {
        .type = ah_packetType_SetProperty,
        .subtype = ah_propertyType_EyeTrackingStreamRate,
        .rate = rateHz,
    };

    return ah_request_sendRequest(&request, sizeof(request), NULL, NULL);
}

ah_result ah_api_getEyeTrackingStreamRate(float *rateHz)
{
    struct __attribute__((__packed__))
    {
        uint8_t type;
        uint8_t subtype;
        float rate;
    } request = {
        .type = ah_packetType_GetProperty,
        .subtype = ah_propertyType_EyeTrackingStreamRate,
    };
    struct __attribute__((__packed__)) responseData
    {
        uint8_t subtype;
        float rate;
    };

    void *response = NULL;

    ah_result rv = ah_request_sendRequest(&request, sizeof(request), &response, NULL);

    if (rv == ah_result_Success)
    {
        struct responseData *data = response;
        *rateHz = data->rate;
    }
    ah_free(response);
    return rv;
}

ah_result ah_api_setEventControl(unsigned eventControlBitmask, bool enable)
{
    struct __attribute__((__packed__))
    {
        uint8_t type;
        uint8_t subtype;
        uint32_t enableBitfield;
        bool enable;
    } request = {
        .type = ah_packetType_SetProperty,
        .subtype = ah_propertyType_EventControl,
        .enableBitfield = eventControlBitmask,
        .enable = enable,
    };

    return ah_request_sendRequest(&request, sizeof(request), NULL, NULL);
}

ah_result ah_api_getEventControl(unsigned *enabledEventBitmask)
{
    struct __attribute__((__packed__))
    {
        uint8_t type;
        uint8_t subtype;
    } request = {
        .type = ah_packetType_GetProperty,
        .subtype = ah_propertyType_EventControl,
    };
    struct __attribute__((__packed__)) responseData
    {
        uint8_t subtype;
        uint32_t enabledBitfield;
    };

    void *responsePtr = NULL;
    ah_result rv = ah_request_sendRequest(&request, sizeof(request), &responsePtr, NULL);
    {
        struct responseData *response = responsePtr;
        memcpy(enabledEventBitmask, &response->enabledBitfield, sizeof(*enabledEventBitmask));
    }
    ah_free(responsePtr);
    return rv;
}

ah_result ah_api_triggerAutotune(ah_autotuneReferenceGazeVector *referenceGazeVector)
{
    struct __attribute__((__packed__))
    {
        uint8_t type;
        float xPos;
        float yPos;
        float zPos;
    } request = {
        .type = ah_packetType_TriggerAutoTune,
    };

    unsigned requestLen = sizeof(request.type);

    if (referenceGazeVector)
    {
        request.xPos = referenceGazeVector->x;
        request.yPos = referenceGazeVector->y;
        request.zPos = referenceGazeVector->z;
        requestLen = sizeof(request);
    }

    return ah_request_sendRequest(&request, requestLen, NULL, NULL);
}

ah_result ah_api_calibrationStart(ah_calibrationMode mode)
{
    struct __attribute__((__packed__))
    {
        uint8_t type;
        uint8_t mode;
    } request = {
        .type = ah_packetType_CalibrationStart,
        .mode = mode,
    };

    return ah_request_sendRequest(&request, sizeof(request), NULL, NULL);
}

ah_result ah_api_calibrationComplete(void)
{
    struct __attribute__((__packed__))
    {
        uint8_t type;
    } request = {
        .type = ah_packetType_CalibrationComplete,
    };

    return ah_request_sendRequest(&request, sizeof(request), NULL, NULL);
}

ah_result ah_api_calibrationAbort(void)
{
    struct __attribute__((__packed__))
    {
        uint8_t type;
    } request = {
        .type = ah_packetType_CalibrationAbort,
    };

    return ah_request_sendRequest(&request, sizeof(request), NULL, NULL);
}

ah_result ah_api_calibrationRegisterSample(unsigned idx)
{
    struct __attribute__((__packed__))
    {
        uint8_t type;
        uint8_t idx;
    } request = {
        .type = ah_packetType_RegisterCalibrationSample,
        .idx = idx,
    };

    return ah_request_sendRequest(&request, sizeof(request), NULL, NULL);
}

ah_result ah_api_calibrationRegisterPoint(unsigned idx, float xPos, float yPos, float zPos)
{
    struct __attribute__((__packed__))
    {
        uint8_t type;
        uint8_t idx;
        float xPos;
        float yPos;
        float zPos;
    } request = {
        .type = ah_packetType_RegisterCalibrationPoint,
        .idx = idx,
        .xPos = xPos,
        .yPos = yPos,
        .zPos = zPos,
    };

    return ah_request_sendRequest(&request, sizeof(request), NULL, NULL);
}

ah_result ah_api_calibrationRecenter(float xPos, float yPos, float zPos)
{
    struct __attribute__((__packed__))
    {
        uint8_t type;
        float xPos;
        float yPos;
        float zPos;
    } request = {
        .type = ah_packetType_CalibrationRecenter,
        .xPos = xPos,
        .yPos = yPos,
        .zPos = zPos,
    };

    return ah_request_sendRequest(&request, sizeof(request), NULL, NULL);
}

ah_result ah_api_validationStart(void)
{
    struct __attribute__((__packed__))
    {
        uint8_t type;
    } request = {
        .type = ah_packetType_ValidationStart,
    };

    return ah_request_sendRequest(&request, sizeof(request), NULL, NULL);
}

ah_result ah_api_validationComplete(void)
{
    struct __attribute__((__packed__))
    {
        uint8_t type;
    } request = {
        .type = ah_packetType_ValidationComplete,
    };

    return ah_request_sendRequest(&request, sizeof(request), NULL, NULL);
}

ah_result ah_api_validationRegisterPoint(float xPos, float yPos, float zPos)
{
    struct __attribute__((__packed__))
    {
        uint8_t type;
        float xPos;
        float yPos;
        float zPos;
    } request = {
        .type = ah_packetType_RegisterValidationPoint,
        .xPos = xPos,
        .yPos = yPos,
        .zPos = zPos,
    };

    return ah_request_sendRequest(&request, sizeof(request), NULL, NULL);
}

ah_result ah_api_getNominalEyeOffsets(float offsets[AH_NUM_EYES][AH_XYZ_SIZE])
{
    struct __attribute__((__packed__))
    {
        uint8_t type;
        uint8_t subtype;
    } request = {
        .type = ah_packetType_GetProperty,
        .subtype = ah_propertyType_NominalEyeOffsets,
    };
    struct __attribute__((__packed__)) responseData
    {
        uint8_t subtype;
        float offsets[AH_NUM_EYES][AH_XYZ_SIZE];
    };

    void *responsePtr = NULL;
    ah_result rv = ah_request_sendRequest(&request, sizeof(request), &responsePtr, NULL);
    if (rv == ah_result_Success)
    {
        struct responseData *response = responsePtr;
        memcpy(offsets, response->offsets, sizeof(response->offsets));
    }
    ah_free(responsePtr);
    return rv;
}

ah_result ah_api_setDataloggingMode(ah_logMode mode)
{
    struct __attribute__((__packed__))
    {
        uint8_t type;
        uint8_t subtype;
        uint8_t mode;
    } setLogModeRequest = {
        .type = ah_packetType_SetProperty,
        .subtype = ah_propertyType_DataloggingMode,
        .mode = mode,
    };
    return ah_request_sendRequest(&setLogModeRequest, sizeof(setLogModeRequest), NULL, NULL);
}
