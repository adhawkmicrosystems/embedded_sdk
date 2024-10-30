#include "tracker_info.h"

#include "ah_api.h"
#include "bsp_alloc.h"
#include "util.h"

#include <stddef.h>
#include <string.h>

struct tracker_info
{
    unsigned eyeMask;
    unsigned cameraType;
    unsigned productId;
    char serialNumber[32];
    char firmwareVersion[32];
    char apiVersion[32];
    bool isUpdater;
};
static struct tracker_info s_trackerInfo;

bool ah_tracker_info_init(void)
{
    ah_trackerInfo types[] = {
        ah_trackerInfo_CameraType,
        ah_trackerInfo_SerialNumber,
        ah_trackerInfo_ApiVersion,
        ah_trackerInfo_FirmwareVersion,
        ah_trackerInfo_EyeMask,
        ah_trackerInfo_ProductId,
        ah_trackerInfo_RunningImageId,
    };
    uint8_t *response = NULL;
    unsigned responseLen;
    ah_result rv = ah_api_getTrackerInfo(types, AH_ARRAY_SIZE(types), &response, &responseLen);
    if (rv != ah_result_Success || !response || response[0] != ah_trackerInfo_Multi)
    {
        if (response)
        {
            ah_free(response);
        }

        return false;
    }

    uint8_t *itr = &response[1];
    while (itr < response + responseLen)
    {
        ah_trackerInfo type = itr[0];
        unsigned len = itr[1];
        uint8_t *data = &itr[2];

        switch (type)
        {
            case ah_trackerInfo_CameraType:
                memcpy(&s_trackerInfo.cameraType, data, len);
                break;
            case ah_trackerInfo_SerialNumber:
                memcpy(s_trackerInfo.serialNumber, data, len);
                break;
            case ah_trackerInfo_ApiVersion:
                memcpy(s_trackerInfo.apiVersion, data, len);
                break;
            case ah_trackerInfo_FirmwareVersion:
                memcpy(s_trackerInfo.firmwareVersion, data, len);
                break;
            case ah_trackerInfo_EyeMask:
                memcpy(&s_trackerInfo.eyeMask, data, len);
                break;
            case ah_trackerInfo_ProductId:
                memcpy(&s_trackerInfo.productId, data, len);
                break;
            case ah_trackerInfo_RunningImageId:
                s_trackerInfo.isUpdater = !data[0];
                break;
            default:
                break;
        }
        itr += 2 + len;
    }

    ah_free(response);

    return true;
}

void ah_tracker_info_deinit(void)
{
    memset(&s_trackerInfo, 0, sizeof(s_trackerInfo));
}

bool ah_tracker_info_reload(void)
{
    ah_tracker_info_deinit();
    return ah_tracker_info_init();
}

unsigned ah_tracker_info_getEyeMask(void)
{
    return s_trackerInfo.eyeMask;
}

unsigned ah_tracker_info_getCameraType(void)
{
    return s_trackerInfo.cameraType;
}

unsigned ah_tracker_info_getProductId(void)
{
    return s_trackerInfo.productId;
}

char *ah_tracker_info_getSerialNumber(void)
{
    return s_trackerInfo.serialNumber;
}

char *ah_tracker_info_getFirmwareVersion(void)
{
    return s_trackerInfo.firmwareVersion;
}

char *ah_tracker_info_getAPIVersion(void)
{
    return s_trackerInfo.apiVersion;
}

bool ah_tracker_info_isUpdater(void)
{
    return s_trackerInfo.isUpdater;
}
