struct VSInput {
    float3 position : POSITION;
    float2 texCoord : TEXCOORD;
};

struct PSInput {
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

cbuffer constants : register(b0)
{
    row_major float4x4 Model;
    row_major float4x4 ViewProj;
    float Flag;
}

PSInput main(VSInput input) {


    PSInput output;
    output.position = mul(float4(input.position, 1.0f), Model);
    output.position = mul(output.position, ViewProj);
    
    output.texCoord = input.texCoord;
    
    return output;
}
