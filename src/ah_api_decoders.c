#include "ah_api_decoders.h"

#include "ah_types.h"
#include "ah_typesEyetracking.h"
#include "ah_vector.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static bool decodeGazeData(ah_gazeStreamData *gaze, const uint8_t **data, uint32_t len)
{
    struct __attribute__((__packed__)) packedData
    {
        float vec[AH_XYZ_SIZE];
        float vergence;
    };

    if (len < sizeof(struct packedData))
    {
        return false;
    }

    struct packedData *packed = (struct packedData *)*data;
    gaze->vec.x = packed->vec[0];
    gaze->vec.y = packed->vec[1];
    gaze->vec.z = packed->vec[2];
    gaze->vergence = packed->vergence;

    *data += sizeof(struct packedData);
    return true;
}

static bool decodePerEyeVector3(ah_vector3 vector[AH_NUM_EYES], const uint8_t **data, uint32_t len, uint8_t eyeMask)
{
    const uint32_t numEyes = eyeMask == 3 ? 2 : 1;
    const uint32_t offset = eyeMask == 2 ? 1 : 0;

    const uint32_t packedSize = sizeof(float[AH_XYZ_SIZE]) * numEyes;
    if (len < packedSize)
    {
        return false;
    }

    if (numEyes != 2)
    {
        // Fill unused eye with NaN
        memset(vector, 0xff, sizeof(ah_vector3) * AH_NUM_EYES);
    }
    memcpy(vector + offset, *data, packedSize);
    *data += packedSize;
    return true;
}

static bool decodePerEyeFloat(float f[AH_NUM_EYES], const uint8_t **data, uint32_t len, uint8_t eyeMask)
{
    const uint32_t numEyes = eyeMask == 3 ? 2 : 1;
    const uint32_t offset = eyeMask == 2 ? 1 : 0;

    const uint32_t packedSize = sizeof(float) * numEyes;
    if (len < packedSize)
    {
        return false;
    }

    if (numEyes != 2)
    {
        // Fill unused eye with NaN
        memset(f, 0xff, sizeof(float) * AH_NUM_EYES);
    }
    memcpy(f + offset, *data, packedSize);
    *data += packedSize;
    return true;
}

static bool decodeGazeInImage(ah_gazeInImageStreamData *gazeInImage, const uint8_t **data, uint32_t len)
{
    struct __attribute__((__packed__)) packedData
    {
        float gaze[AH_XY_SIZE];
        float degToPix[AH_XY_SIZE];
    };
    if (len < sizeof(struct packedData))
    {
        return false;
    }

    struct packedData *packed = (struct packedData *)*data;
    gazeInImage->gaze.x = packed->gaze[0];
    gazeInImage->gaze.y = packed->gaze[1];
    gazeInImage->degToPix.x = packed->degToPix[1];
    gazeInImage->degToPix.y = packed->degToPix[1];
    *data += sizeof(struct packedData);
    return true;
}

static bool decodeVector2(ah_vector2 *vector, const uint8_t **data, uint32_t len)
{
    const uint32_t packedSize = sizeof(float) * 2;
    if (len < packedSize)
    {
        return false;
    }

    memcpy(vector, *data, packedSize);
    *data += packedSize;
    return true;
}

static bool decodeQuaternion(ah_imuQuaternionStreamData *quaternion, const uint8_t **data, uint32_t len)
{
    const uint32_t packedSize = sizeof(float) * 4;
    if (len < packedSize)
    {
        return false;
    }

    memcpy(quaternion, *data, packedSize);
    *data += packedSize;
    return true;
}

ah_result ah_api_decodeEyeTrackingStream(ah_eyeTrackingStreamData *stream, const uint8_t *data, uint32_t len)
{
    struct __attribute__((__packed__))
    {
        uint8_t type;
        uint64_t timestamp;
        uint8_t eyeMask;
    } header;

    if (len <= sizeof(header))
    {
        return ah_result_Failure;
    }
    // Set all valid bools to false
    memset(stream, 0, sizeof(*stream));

    memcpy(&header, data, sizeof(header));
    stream->timestamp = header.timestamp;
    stream->eyeMask = header.eyeMask;

    const uint8_t *p = data + sizeof(header);
    while (p < data + len)
    {
        ah_eyeTrackingStreamType type = *p++;
        const ptrdiff_t unparsedLen = (data + len) - p;

        switch (type)
        {
            case ah_eyeTrackingStreamType_Gaze:
            {
                if (!decodeGazeData(&stream->gaze, &p, unparsedLen))
                {
                    return ah_result_Failure;
                }
                stream->gazeEnabled = true;
                break;
            }
            case ah_eyeTrackingStreamType_PerEyeGaze:
            {
                if (!decodePerEyeVector3(stream->perEyeGaze, &p, unparsedLen, stream->eyeMask))
                {
                    return ah_result_Failure;
                }
                stream->perEyeGazeEnabled = true;
                break;
            }
            case ah_eyeTrackingStreamType_EyeCenter:
            {
                if (!decodePerEyeVector3(stream->eyeCenter, &p, unparsedLen, stream->eyeMask))
                {
                    return ah_result_Failure;
                }
                stream->eyeCenterEnabled = true;
                break;
            }
            case ah_eyeTrackingStreamType_PupilPosition:
            {
                if (!decodePerEyeVector3(stream->pupilPos, &p, unparsedLen, stream->eyeMask))
                {
                    return ah_result_Failure;
                }
                stream->pupilPosEnabled = true;
                break;
            }
            case ah_eyeTrackingStreamType_PupilDiameter:
            {
                if (!decodePerEyeFloat(stream->pupilDiameter, &p, unparsedLen, stream->eyeMask))
                {
                    return ah_result_Failure;
                }
                stream->pupilDiameterEnabled = true;
                break;
            }
            case ah_eyeTrackingStreamType_GazeInImage:
            {
                if (!decodeGazeInImage(&stream->gazeInImage, &p, unparsedLen))
                {
                    return ah_result_Failure;
                }
                stream->gazeInImageEnabled = true;
                break;
            }
            case ah_eyeTrackingStreamType_GazeInScreen:
            {
                if (!decodeVector2(&stream->gazeInScreen, &p, unparsedLen))
                {
                    return ah_result_Failure;
                }
                stream->gazeInScreenEnabled = true;
                break;
            }
            case ah_eyeTrackingStreamType_ImuQuaternion:
            {
                if (!decodeQuaternion(&stream->imuQuaternion, &p, unparsedLen))
                {
                    return ah_result_Failure;
                }
                stream->imuQuaternionEnabled = true;
                break;
            }
            default:
            {
                return ah_result_NotSupported;
            }
        }
    }
    return ah_result_Success;
}
