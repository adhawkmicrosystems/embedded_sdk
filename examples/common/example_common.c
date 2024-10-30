#include "ah_sdk.h"

#include "bsp_logging.h"

#include <inttypes.h>
#include <stddef.h>

static inline const char *eye2str(unsigned eye)
{
    return eye == 0 ? "Right" : "Left";
}

// API calls can't be made in this callback. etData is not valid outside this scope
static void eyeTrackingStreamCallback(const ah_eyeTrackingStreamData *etData, void *ctx)
{
    // Downsample print statements to once per second
    static int64_t s_lastTimestamp;
    if (etData->timestamp < s_lastTimestamp + 1e9f)
    {
        return;
    }
    s_lastTimestamp = etData->timestamp;

    ah_log_info("Eye Tracking Data: timestamp: %" PRId64 ", eye_mask: %#2x (right eye, left eye)",
                etData->timestamp, etData->eyeMask);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdouble-promotion"
    if (etData->gazeEnabled)
    {
        ah_log_info("\tGaze Data: (x=%.4f, y=%.4f, z=%.4f, vergence=%.4f)",
                    etData->gaze.vec.x, etData->gaze.vec.y, etData->gaze.vec.z, etData->gaze.vergence);
    }
    if (etData->perEyeGazeEnabled)
    {
        ah_log_info("\tPer Eye Gaze: (x=%.4f, y=%.4f, z=%.4f), (x=%.4f, y=%.4f, z=%.4f)",
                    etData->perEyeGaze[0].x, etData->perEyeGaze[0].y, etData->perEyeGaze[0].z,
                    etData->perEyeGaze[1].x, etData->perEyeGaze[1].y, etData->perEyeGaze[1].z);
    }
    if (etData->eyeCenterEnabled)
    {
        ah_log_info("\tEye Centers: (x=%.4f, y=%.4f, z=%.4f), (x=%.4f, y=%.4f, z=%.4f)",
                    etData->eyeCenter[0].x, etData->eyeCenter[0].y, etData->eyeCenter[0].z,
                    etData->eyeCenter[1].x, etData->eyeCenter[1].y, etData->eyeCenter[1].z);
    }
    if (etData->pupilPosEnabled)
    {
        ah_log_info("\tPupil Pos:(x=%.4f, y=%.4f, z=%.4f), (x=%.4f, y=%.4f, z=%.4f)",
                    etData->pupilPos[0].x, etData->pupilPos[0].y, etData->pupilPos[0].z,
                    etData->pupilPos[1].x, etData->pupilPos[1].y, etData->pupilPos[1].z);
    }
    if (etData->pupilDiameterEnabled)
    {
        ah_log_info("\tPupil Diameter: (%.4f), (%.4f)",
                    etData->pupilDiameter[0], etData->pupilDiameter[1]);
    }
#pragma GCC diagnostic pop
}

// API calls can't be made in this callback. event is not valid outside this scope
static void eventCallback(const ah_eventStreamData *event, void *ctx)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdouble-promotion"
    switch (event->type)
    {
        case ah_eventType_Blink:
            ah_log_info("[%f] Blink: %f", event->timestamp, event->data.blink.duration);
            break;
        case ah_eventType_Saccade:
            ah_log_info("[%f] Saccade: duration: %f, amplitude: %f, angle: %f, peak_vel: %f",
                        event->timestamp,
                        event->data.saccade.duration,
                        event->data.saccade.amplitude,
                        event->data.saccade.angle,
                        event->data.saccade.peakVel);
            break;
        case ah_eventType_TracklossStart:
            ah_log_info("[%f] %s Eye Trackloss Start", event->timestamp, eye2str(event->data.trackloss.eyeId));
            break;
        case ah_eventType_TracklossEnd:
            ah_log_info("[%f] %s Eye Trackloss End", event->timestamp, eye2str(event->data.trackloss.eyeId));
            break;
        default:
            break;
    }
#pragma GCC diagnostic pop
}

void adhawk_example_run(void)
{
    if (!ah_sdk_init(NULL))
    {
        ah_log_error("Tracker failed to connect");
        return;
    }
    else
    {
        ah_log_info("Tracker successfully connected!");
        ah_log_info("Tracker info: %s: %s v%s", ah_sdk_trackerInfo_getSerialNumber(),
                    ah_sdk_trackerInfo_getFirmwareVersion(),
                    ah_sdk_trackerInfo_getAPIVersion());
    }

    uint8_t rv;

    rv = ah_sdk_enableTracking(true);
    if (rv != ah_result_Success)
    {
        ah_log_error("Error enabling eye tracking: %s", ah_sdk_resultToString(rv));
    }

    rv = ah_sdk_runAutotune(NULL);
    if (rv != ah_result_Success)
    {
        ah_log_info("Error running autotune: %s", ah_sdk_resultToString(rv));
    }

    rv = ah_sdk_setEyeTrackingDataCallback(eyeTrackingStreamCallback, NULL);
    if (rv != ah_result_Success)
    {
        ah_log_error("Error setting callback: %s", ah_sdk_resultToString(rv));
    }

    uint32_t enableMask = ah_eyeTrackingStreamControl_Gaze |
                          ah_eyeTrackingStreamControl_PerEyeGaze |
                          ah_eyeTrackingStreamControl_EyeCenter |
                          ah_eyeTrackingStreamControl_PupilPosition |
                          ah_eyeTrackingStreamControl_PupilDiameter;

    rv = ah_sdk_eyeTrackingStreamControl(enableMask, true);
    if (rv != ah_result_Success)
    {
        ah_log_error("Error setting streams to %#x: %s", (unsigned)enableMask, ah_sdk_resultToString(rv));
    }

    rv = ah_sdk_setEyeTrackingRate(ah_eyeTrackingRate_60Hz);
    if (rv != ah_result_Success)
    {
        ah_log_error("Error setting eye tracking rate: %s", ah_sdk_resultToString(rv));
    }

    rv = ah_sdk_setupEvent(ah_eventControl_Blink, true, eventCallback, NULL);
    if (rv != ah_result_Success)
    {
        ah_log_error("Error enabling blink events: %d", rv);
    }

    rv = ah_sdk_setupEvent(ah_eventControl_Saccade, true, eventCallback, NULL);
    if (rv != ah_result_Success)
    {
        ah_log_info("Error enabling saccade events: %d", rv);
    }

    rv = ah_sdk_setupEvent(ah_eventControl_TracklossStartEnd, true, eventCallback, NULL);
    if (rv != ah_result_Success)
    {
        ah_log_error("Error enabling trackloss start / stop events: %d", rv);
    }

    ah_log_info("Tracker Status: %d", ah_sdk_getTrackerStatus());
}
