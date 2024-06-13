#version 460

struct InputStruct
{
    uint format;
    uint width;
    uint height;
    uint mipLevel;
    uint layerCount;
    uint Temp0;     //temp2
};

struct OutputStruct
{
    uint Temp1;
    uint Temp2;
    uint Temp3;
    uint Temp4;
};

layout (set = 0, binding = 0) readonly buffer InputStructBuffer
{
    InputStruct data;
} InputStructInfo;


