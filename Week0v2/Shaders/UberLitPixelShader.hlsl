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

struct PS_Input 
{

};

float4 mainPS() : SV_TARGET
{
    //float4 finalPixel = TextureColor;
    //finalPixel += Emissive;
#if LIGHTING_MODEL_GOURAUD
#elif LIGHTING_MODEL_LAMBERT
    finalPixel += CalculateAmbientLight(...);
    for(It : PointLights)
    {
        finalPixel += CalculatePointLight(...);
    }
#elif LIGHTING_MODEL_PHONG
    // Specular Reflectance
#endif
    //return finalPixel;
}