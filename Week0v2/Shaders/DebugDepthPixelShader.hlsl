#include "ShaderHeaders/GSamplers.hlsli"
Texture2D<float> SceneDepthTex : register(t0);

cbuffer FCameraConstant : register(b0)
{
    matrix ViewMatrix;
    matrix ProjMatrix;
    matrix InvViewMatrix;
    matrix InvProjMatrix;
    
    float3 CameraPos;
    float NearPlane;
    float3 CameraForward;
    float FarPlane;
};

cbuffer FViewportInfo : register(b1)
{
    float2 ViewportSize;
    float2 ViewportOffset;
}

struct VS_OUT
{
    float4 position : SV_POSITION; // 변환된 화면 좌표
    float2 uv : TEXCOORD0; // UV 좌표
};

float4 mainPS(VS_OUT input) : SV_Target
{
    float2 viewportUV = input.uv * ViewportSize + ViewportOffset;
    
    float depth = SceneDepthTex.Sample(pointSampler, viewportUV).r;
    if (depth == 1.0)
    {
        return float4(0, 0, 0, 1);
    }

    float linearDepth = (NearPlane * FarPlane) / (FarPlane - depth * (FarPlane - NearPlane));
    float normalized = saturate((linearDepth - NearPlane) / (FarPlane - NearPlane));

    float expo = 1 - exp(-normalized * 1.0);


    return float4(expo, expo, expo, 1.0);
}