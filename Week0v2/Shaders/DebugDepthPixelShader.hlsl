#include "ShaderHeaders/GSamplers.hlsli"
Texture2D SceneDepthTex : register(t0);

cbuffer FCameraConstant : register(b0)
{
    matrix ViewMatrix;
    matrix ProjMatrix;
    matrix ViewProjMatrix;
    
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

//    float linearDepth = (0.1 * 1000) / (1000 - depth * (1000 - 0.1));
    float linearDepth = (NearPlane * FarPlane) / (FarPlane - depth * (FarPlane - NearPlane));
    
    float normalized = saturate((linearDepth - 0.1) / (1000 - 0.1));

    return float4(normalized, normalized, normalized, 1.0);
}