cbuffer LightMatrix : register(b0)
{
    row_major float4x4 Model;
    row_major float4x4 View;
    row_major float4x4 Proj;
}

struct VS_INPUT
{
    float4 position : POSITION; // 버텍스 위치
    float4 color : COLOR; // 버텍스 색상
    float3 normal : NORMAL; // 버텍스 노멀
    float3 tangent : TANGENT;
    float2 texcoord : TEXCOORD;
};

float4 mainVS(VS_INPUT input) : SV_POSITION
{
    float4 ModelPos = mul(input.position, Model);
    float4 ViewPos = mul(ModelPos, View);
    float4 FinalPos = mul(ViewPos, Proj);
    return FinalPos;
}