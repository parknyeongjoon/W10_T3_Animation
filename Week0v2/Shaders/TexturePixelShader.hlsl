#include "ShaderHeaders/GSamplers.hlsli"
Texture2D gTexture : register(t0);

cbuffer FSubUVConstant : register(b1)
{
    float indexU;
    float indexV;
}

cbuffer FUUIDConstant : register(b2)
{
    float4 UUID;
}

struct PSInput 
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

struct PSOutput
{
    float4 color : SV_Target0;
    float4 uuid : SV_Target1;
};

float4 mainPS(PSInput input) : SV_TARGET {
    PSOutput output;
    
    float2 uv = input.texCoord + float2(indexU, indexV);
    //float4 col = gTexture.Sample(gSampler, input.texCoord);
    float4 col = gTexture.Sample(linearSampler, uv);
    float threshold = 0.05; // 필요한 경우 임계값을 조정
    if (col.r < threshold && col.g < threshold && col.b < threshold)
        clip(-1); // 픽셀 버리기
    
    output.color = col;
    output.uuid = UUID;
    
    return col;
}
