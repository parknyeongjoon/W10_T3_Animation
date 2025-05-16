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

    float moment1 = depth;
    //float dx = ddx(moment1);
    //float dy = ddy(moment1);
    //float moment2 = moment1 * moment1 + 0.25f * (dx * dx + dy * dy);
    float moment2 = depth * depth;

    return float4(moment1, moment2, 0.0, 1.0);
}