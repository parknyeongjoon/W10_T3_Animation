struct VS_OUT
{
    float4 position : SV_POSITION; // 변환된 화면 좌표
    float2 uv : TEXCOORD0; // UV 좌표
};

VS_OUT mainVS(uint id : SV_VertexID)
{
    VS_OUT output;

    float2 pos[4] =
    {
        float2(-1, 1), // LT
        float2(1, 1), // RT
        float2(-1, -1), // LB
        float2(1, -1) // RB
    };
    
    float2 uv[4] =
    {
        float2(0, 0),
        float2(1, 0),
        float2(0, 1),
        float2(1, 1)
    };
    
    output.position = float4(pos[id], 0.0, 1.0);
    output.uv = uv[id];
    
    return output;
}
