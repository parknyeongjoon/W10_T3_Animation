#include "ShaderHeaders/GConstantBuffers.hlsli"

Texture2D FinalTexture : register(t0);        // 핑퐁 버퍼의 최종 결과
SamplerState PointSampler : register(s0);     // 포인트 필터링 샘플러

struct VS_OUT
{
    float4 position : SV_POSITION; // 변환된 화면 좌표
    float2 uv : TEXCOORD0; // UV 좌표
};

float4 mainPS(VS_OUT input) : SV_TARGET
{
    // 1. 원본 씬 텍스처에서 색상 샘플링
    float2 viewportUV = input.uv * ViewportSize + ViewportOffset;
    float4 sceneColor = FinalTexture.Sample(PointSampler, viewportUV);

    return float4(sceneColor.xyz, 1.0);
}