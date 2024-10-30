#pragma once

typedef union
{
    struct
    {
        float x;
        float y;
    };
    float dim[2];
} ah_vector2;

typedef union
{
    struct
    {
        float x;
        float y;
        float z;
    };
    float dim[3];
} ah_vector3;

typedef union
{
    struct
    {
        float x;
        float y;
        float z;
        float w;
    };
    float dim[4];
} ah_quaternion;
