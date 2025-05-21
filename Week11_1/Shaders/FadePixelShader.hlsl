
#include "ShaderHeaders/GConstantBuffers.hlsli"

cbuffer FadeConstants : register(b0) // 레지스터 슬롯은 엔진의 다른 버퍼와 겹치지 않게 선택 (b0는 예시)
{
    float4 TargetFadeColor; // 페이드 목표 색상 (예: 검은색 float4(0,0,0,1))
    
    float  FadeAlpha;       // 보간 계수 (0.0 = SceneColor, 1.0 = TargetFadeColor)
    float3 Padding;      // 필요시 패딩 추가 (float 뒤에 float4 등이 올 경우 메모리 정렬 문제 방지)
};

Texture2D SceneTexture : register(t1);
SamplerState SamplerLinear : register(s0);

struct VS_OUT
{
    float4 position : SV_POSITION; // 변환된 화면 좌표
    float2 uv : TEXCOORD0; // UV 좌표
};


float4 mainPS(VS_OUT input) : SV_TARGET
{
    // 1. 원본 씬 텍스처에서 색상 샘플링
    float2 viewportUV = input.uv * ViewportSize + ViewportOffset;
    float4 sceneColor = SceneTexture.Sample(SamplerLinear, viewportUV);

    // 2. cbuffer에서 받아온 값들을 이용해 최종 색상 계산 (선형 보간)
    // sceneColor와 TargetFadeColor 사이를 FadeAlpha 값으로 보간합니다.
    float4 FinalColor = lerp(sceneColor, TargetFadeColor, FadeAlpha);
    
    return float4(FinalColor.xyz, 1.0);
}
