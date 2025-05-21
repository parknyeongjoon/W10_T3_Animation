#include "ShaderHeaders/GSamplers.hlsli"

#if RENDER_GIZMO
struct PS_INPUT
{
    float4 position : SV_Position;
    float4 color : COLOR;
};

float4 mainPS(PS_INPUT input) : SV_Target
{
    return input.color;
}
#endif

#if RENDER_ICON // 1 : ICON
Texture2D gTexture : register(t0);
struct PS_INPUT
{
    float4 Position : SV_Position;
    float2 TexCoord : TEXCOORD;
};

// 픽셀 셰이더
float4 mainPS(PS_INPUT input) : SV_Target
{
    float4 col = gTexture.Sample(linearSampler, input.TexCoord);
    float threshold = 0.01; // 필요한 경우 임계값을 조정
    if (col.a < threshold)
        clip(-1); // 픽셀 버리기
    
    return col;
}
#endif

#if RENDER_ARROW // 2 : Arrow
float4 arrowPS(PS_INPUT input) : SV_Target
{
    return input.color;
}
#endif