#pragma once

#include "ah_vector.h"

#include <stdbool.h>
#include <stdint.h>

#define AH_NUM_EYES (2)
#define AH_XY_SIZE (2)
#define AH_XYZ_SIZE (3)
#define AH_MAX_NUM_GLINT_DETECTORS (6)

// clang-format off
typedef enum
{
    ah_result_Success                       = 0,
    ah_result_Failure                       = 1,
    ah_result_InvalidArgument               = 2,
    ah_result_TrackerNotReady               = 3,
    ah_result_EyesNotFound                  = 4,
    ah_result_RightEyeNotFound              = 5,
    ah_result_LeftEyeNotFound               = 6,
    ah_result_NotCalibrated                 = 7,
    ah_result_NotSupported                  = 8,
    ah_result_SessionAlreadyRunning         = 9,
    ah_result_NoCurrentSession              = 10,
    ah_result_RequestTimeout                = 11,
    ah_result_UnexpectedResponse            = 12,
    ah_result_HardwareFault                 = 13,
    ah_result_CameraFault                   = 14,
    ah_result_Busy                          = 15,
    ah_result_CommunicationError            = 16,
    ah_result_DeviceCalibrationRequired     = 17,
    ah_result_ProcessIncomplete             = 18,
    ah_result_InactiveInterface             = 19,
    ah_result_TrackerDisconnected           = 20,
} ah_result;

typedef enum
{
    ah_packetType_EyeTrackingStream         = 0x01,
    ah_packetType_TrackersReady             = 0x02,
    ah_packetType_FeatureStream             = 0x09,

    ah_packetType_TrackerStatus             = 0x16,
    ah_packetType_ImuStream                 = 0x17,
    ah_packetType_EventStream               = 0x18,
    ah_packetType_IrisImageDataStream       = 0x20,
    ah_packetType_ConfigDumpStream          = 0x21,
    ah_packetType_McuTemperatureStream      = 0x22,

    ah_packetType_CalibrationStart          = 0x71,
    ah_packetType_CalibrationComplete       = 0x72,
    ah_packetType_CalibrationAbort          = 0x73,
    ah_packetType_RegisterCalibrationSample = 0x74,
    ah_packetType_RegisterCalibrationPoint  = 0x75,

    ah_packetType_TriggerAutoTune           = 0x85,
    ah_packetType_ValidationStart           = 0x86,
    ah_packetType_ValidationComplete        = 0x87,
    ah_packetType_RegisterValidationPoint   = 0x88,

    ah_packetType_LogAnnotation             = 0x8d,
    ah_packetType_CalibrationRecenter       = 0x8f,

    ah_packetType_GetTrackerStatus          = 0x90,
    ah_packetType_DeleteBlob                = 0x91,
    ah_packetType_BlobSize                  = 0x92,
    ah_packetType_BlobData                  = 0x93,
    ah_packetType_LoadBlob                  = 0x94,
    ah_packetType_SaveBlob                  = 0x95,
    ah_packetType_StartLogSession           = 0x96,
    ah_packetType_StopLogSession            = 0x97,
    ah_packetType_GetBackendVersion         = 0x98,
    ah_packetType_DeviceInfo                = 0x99,
    ah_packetType_GetProperty               = 0x9a,
    ah_packetType_SetProperty               = 0x9b,
    ah_packetType_SystemControl             = 0x9c,
    ah_packetType_IrisTriggerCapture        = 0x9d,
    ah_packetType_TriggerRegisterDump       = 0x9e,
    ah_packetType_GetNRFVersion             = 0x9f,

    ah_packetType_ProcedureStart            = 0xb0,
    ah_packetType_ProcedureStatus           = 0xb1,

    ah_packetType_UDPConnection             = 0xc0,
    ah_packetType_UDPDisconnect             = 0xc2,
    ah_packetType_UDPPing                   = 0xc5,
} ah_packetType;

typedef enum
{
    ah_propertyType_StreamControl           = 2,
    ah_propertyType_ComponentOffsets        = 4,
    ah_propertyType_EventControl            = 5,
    ah_propertyType_EyeTrackingStreamRate   = 13,
    ah_propertyType_EyeTrackingStreams      = 14,
    ah_propertyType_FeatureStreams          = 15,
    ah_propertyType_NominalEyeOffsets       = 17,
    ah_propertyType_DataloggingMode         = 19,
} ah_propertyType;

typedef enum
{
    ah_systemControlType_EyeTracking        = 1,
    ah_systemControlType_SocControl         = 2,
} ah_systemControlType;

typedef enum
{
    ah_streamControl_McuTemperature         = (1u << 22),
    ah_streamControl_ImuRotation            = (1u << 23),
    ah_streamControl_Imu                    = (1u << 31),
} ah_streamControl;

typedef enum
{
    ah_procedureType_DeviceCalibration      = 1,
    ah_procedureType_UpdateFirmware         = 2,
} ah_procedureType;

typedef enum
{
    ah_blobType_Calibration                 = 1,
    ah_blobType_Multiglint                  = 2,
    ah_blobType_Autotune                    = 3,
    ah_blobType_DynamicFusion               = 4,
    ah_blobType_ModuleCal                   = 5,
    ah_blobType_ModelET                     = 6,
    ah_blobType_ModelPriors                 = 7,
    ah_blobType_GeometryData                = 8,
    ah_blobType_AutotuneMultiglint          = 9,
    ah_blobType_Personalization             = 10,

    ah_blobType_Count
} ah_blobType;

typedef enum
{
    ah_eventType_Blink                      = 1,
    ah_eventType_EyeClosed                  = 2,
    ah_eventType_EyeOpened                  = 3,
    ah_eventType_TracklossStart             = 4,
    ah_eventType_TracklossEnd               = 5,
    ah_eventType_Saccade                    = 6,
    ah_eventType_PerEyeSaccadeStart         = 7,
    ah_eventType_PerEyeSaccadeEnd           = 8,
    ah_eventType_ValidationSample           = 9,
    ah_eventType_ValidationSummary          = 10,
    ah_eventType_ProcedureStarted           = 11,
    ah_eventType_ProcedureEnded             = 12,
    ah_eventType_ExternalTrigger            = 13,
    ah_eventType_GazeDepth                  = 14,
    ah_eventType_Fixation                   = 15,
    ah_eventType_FixationStart              = 16,
} ah_eventType;

typedef enum
{
    ah_eventControl_Blink                   = (1u << 0),
    ah_eventControl_EyeOpenClose            = (1u << 1),
    ah_eventControl_TracklossStartEnd       = (1u << 2),
    ah_eventControl_Saccade                 = (1u << 3),
    ah_eventControl_PerEyeSaccadeStartEnd   = (1u << 4),
    ah_eventControl_ValidationResults       = (1u << 5),
    ah_eventControl_ProcedureStartEnd       = (1u << 6),
    ah_eventControl_ExternalTrigger         = (1u << 7),
    ah_eventControl_GazeDepth               = (1u << 8),
    ah_eventControl_Fixation                = (1u << 9),
} ah_eventControl;

typedef enum
{
    ah_calibrationMode_Standard            = 1,
    ah_calibrationMode_IMU                 = 2,
} ah_calibrationMode;

typedef enum
{
    ah_logMode_None                         = 1,  //! Disable data logging
    ah_logMode_Basic                        = 2,  //! Device configs and tuning documents
    ah_logMode_Ocular                       = 3,  //! Basic + documents (autophase, autotune, calibration, validation)
    ah_logMode_DiagnosticsLite              = 4,  //! Ocular + streams + eye features + accepted pulses
    ah_logMode_DiagnosticsFull              = 5,  //! Ocular + streams + eye features + all pulses
} ah_logMode;

typedef enum
{
    ah_trackerInfo_CameraType               = 1,
    ah_trackerInfo_SerialNumber             = 2,
    ah_trackerInfo_ApiVersion               = 3,
    ah_trackerInfo_FirmwareVersion          = 4,
    ah_trackerInfo_EyeMask                  = 5,
    ah_trackerInfo_ProductId                = 6,
    ah_trackerInfo_Multi                    = 7,
    ah_trackerInfo_RunningImageId           = 8,
} ah_trackerInfo;

typedef enum
{
    ah_eyeTrackingStreamType_Gaze = 1,
    ah_eyeTrackingStreamType_PerEyeGaze,
    ah_eyeTrackingStreamType_EyeCenter,
    ah_eyeTrackingStreamType_PupilPosition,
    ah_eyeTrackingStreamType_PupilDiameter,
    ah_eyeTrackingStreamType_GazeInImage,   //! Not available in the Embedded SDK
    ah_eyeTrackingStreamType_GazeInScreen,  //! Not available in the Embedded SDK
    ah_eyeTrackingStreamType_ImuQuaternion,

    ah_eyeTrackingStreamType_Count
} ah_eyeTrackingStreamType;

typedef enum
{
    ah_eyeTrackingStreamControl_Gaze = (1u << ah_eyeTrackingStreamType_Gaze),
    ah_eyeTrackingStreamControl_PerEyeGaze = (1u << ah_eyeTrackingStreamType_PerEyeGaze),
    ah_eyeTrackingStreamControl_EyeCenter = (1u << ah_eyeTrackingStreamType_EyeCenter),
    ah_eyeTrackingStreamControl_PupilPosition = (1u << ah_eyeTrackingStreamType_PupilPosition),
    ah_eyeTrackingStreamControl_PupilDiameter = (1u << ah_eyeTrackingStreamType_PupilDiameter),
    ah_eyeTrackingStreamControl_GazeInImage = (1u << ah_eyeTrackingStreamType_GazeInImage),
    ah_eyeTrackingStreamControl_GazeInScreen = (1u << ah_eyeTrackingStreamType_GazeInScreen),
    ah_eyeTrackingStreamControl_ImuQuaternion = (1u << ah_eyeTrackingStreamType_ImuQuaternion),
} ah_eyeTrackingStreamControl;
// clang-format on

typedef ah_vector3 ah_autotuneReferenceGazeVector;

typedef struct __attribute__((__packed__))
{
    float duration;
} ah_blinkEventData;

typedef struct __attribute__((__packed__))
{
    uint8_t eyeId;
} ah_eyeOpenCloseEventData;

typedef struct __attribute__((__packed__))
{
    float duration;
    float amplitude;
    float angle;
    float peakVel;
} ah_saccadeEventData;

typedef struct __attribute__((__packed__))
{
    uint8_t eyeId;
} ah_perEyeSaccadeStartEventData;

typedef struct __attribute__((__packed__))
{
    uint8_t eyeId;
    float duration;
    float amplitude;
    float angle;
    float peakVel;
} ah_perEyeSaccadeEndEventData;

typedef struct __attribute__((__packed__))
{
    float depth;
} ah_gazeDepthEventData;

typedef struct __attribute__((__packed__))
{
    uint8_t eyeId;
} ah_tracklossEventData;

typedef struct __attribute__((__packed__))
{
    float duration;
    float avg_gaze[AH_XYZ_SIZE];
    float avg_speed;
    float std_gaze;
} ah_fixationEventData;

typedef struct __attribute__((__packed__))
{
    uint8_t eyeId;
    float gaze[AH_XYZ_SIZE];
} ah_fixationStartEventData;

typedef struct __attribute__((__packed__))
{
    ah_eventType type : 8;
    float timestamp;
    union
    {
        ah_blinkEventData blink;
        ah_eyeOpenCloseEventData eyeOpenClose;
        ah_saccadeEventData saccade;
        ah_perEyeSaccadeStartEventData perEyeSaccadeStart;
        ah_perEyeSaccadeEndEventData perEyeSaccadeEnd;
        ah_gazeDepthEventData gazeDepth;
        ah_tracklossEventData trackloss;
        ah_fixationEventData fixation;
        ah_fixationStartEventData fixationStart;
    } data;
} ah_eventStreamData;

typedef struct __attribute__((__packed__))
{
    float timestamp;    //! Seconds since boot
    float temperature;  // In degrees Celsius
} ah_mcuTemperatureStreamData;
