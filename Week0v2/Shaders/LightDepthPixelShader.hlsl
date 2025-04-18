#include "ShaderHeaders/GSamplers.hlsli"
Texture2D<float> SceneDepthTex : register(t0);

struct VS_OUT
{
    float4 position : SV_POSITION; // 변환된 화면 좌표
    float2 uv : TEXCOORD0; // UV 좌표
};

float4 mainPS(VS_OUT input) : SV_Target
{
    float depth = SceneDepthTex.Sample(pointSampler, input.uv).r;

    float linearDepth = (0.1 * 100) / (100 - depth * (100 - 0.1));
    
    float normalized = saturate((linearDepth - 0.1) / (100 - 0.1));

    return float4(normalized, normalized, normalized, 1.0);
}