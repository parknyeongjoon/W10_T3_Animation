#include "ShaderHeaders/GSamplers.hlsli"
#include "ShaderHeaders/GConstantBuffers.hlsli"

#define NUM_POINT_LIGHT 4
#define NUM_SPOT_LIGHT 4

struct FAmbientLightInfo
{
};

struct FDirectionalLightInfo
{
};

struct FPointLightInfo
{    
};

struct FSpotLightInfo
{    
};

cbuffer PerObject : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
}; 

cbuffer Lighting : register(b1)
{
    FAmbientLightInfo Ambient;
    FDirectionalLightInfo Directional;
    FPointLightInfo PointLights[NUM_POINT_LIGHT];
    FSpotLightInfo SpotLights[NUM_SPOT_LIGHT];
};

float4 CalculateAmbientLight(FAmbientLightInfo info)
{
    float4 result;
    return result;
}

struct VS_Input
{

};

struct PS_Input 
{

};

PS_Input mainVS(VS_Input Input)
{
    PS_Input out;
#if LIGHTING_MODEL_GOURAUD

#elif LIGHTING_MODEL_LAMBERT

#elif LIGHTING_MODEL_PHONG

#endif
}
