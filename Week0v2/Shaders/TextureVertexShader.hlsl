struct VS_Input
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORD;
};

struct PS_Input 
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

cbuffer FConstants : register(b0)
{
    row_major float4x4 MVP;
    float Flag;
}

PS_Input mainVS(VS_Input input) {
    
    PS_Input output;
    output.position = mul(float4(input.position, 1.0f), MVP);
    output.texCoord = input.texCoord;
    
    return output;
}
