#pragma once

#include "ah_types.h"
#include "ah_vector.h"

#include <stdbool.h>
#include <stdint.h>

//! Eye Tracking Stream Types

//! Gaze vector format:
//! x: Positive values are to the right, negative values are left
//! y: Positive values are up, negative values down
//! z: Positive values behind the user, negative values are in front

typedef struct
{
    ah_vector3 vec;
    float vergence;  //! Vergence angle
} ah_gazeStreamData;

typedef ah_vector3 ah_perEyeGazeStreamData[AH_NUM_EYES];

typedef ah_vector3 ah_eyeCenterStreamData[AH_NUM_EYES];

//! All values are in millimeters
typedef ah_vector3 ah_pupilPositionStreamData[AH_NUM_EYES];

typedef float ah_pupilDiameterStreamData[AH_NUM_EYES];

typedef struct
{
    //! X,Y value of the gaze
    ah_vector2 gaze;
    //! X,Y degrees to pixels
    ah_vector2 degToPix;
} ah_gazeInImageStreamData;

typedef ah_vector2 ah_gazeInScreenStreamData;

typedef ah_quaternion ah_imuQuaternionStreamData;

typedef struct
{
    int64_t timestamp;  //! Nanoseconds since boot
    uint8_t eyeMask;    //! 0x01 == right eye, 0x10 == left eye, 0x11 == both eyes

    bool gazeEnabled;
    ah_gazeStreamData gaze;

    bool perEyeGazeEnabled;
    ah_perEyeGazeStreamData perEyeGaze;

    bool eyeCenterEnabled;
    ah_eyeCenterStreamData eyeCenter;

    bool pupilPosEnabled;
    ah_pupilPositionStreamData pupilPos;

    bool pupilDiameterEnabled;
    ah_pupilDiameterStreamData pupilDiameter;

    //! Not available in the Embedded SDK
    bool gazeInImageEnabled;
    ah_gazeInImageStreamData gazeInImage;

    //! Not available in the Embedded SDK
    bool gazeInScreenEnabled;
    ah_gazeInScreenStreamData gazeInScreen;

    bool imuQuaternionEnabled;
    ah_imuQuaternionStreamData imuQuaternion;
} ah_eyeTrackingStreamData;
