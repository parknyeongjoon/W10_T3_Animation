struct VS_OUT
{
    float4 position : SV_POSITION; // 변환된 화면 좌표
    float2 uv : TEXCOORD0; // UV 좌표
};

VS_OUT mainVS(uint vertexID : SV_VertexID)
{
    VS_OUT output;

    // 정점 위치 (NDC 좌표계)
    float2 ndcPositions[6] =
    {
        float2(-1.0f, 1.0f), // top-left     (0)
        float2(1.0f, 1.0f), // top-right    (1)
        float2(-1.0f, -1.0f), // bottom-left  (2)

        float2(-1.0f, -1.0f), // bottom-left  (3)
        float2(1.0f, 1.0f), // top-right    (4)
        float2(1.0f, -1.0f) // bottom-right (5)
    };

    // 정점에 대응하는 UV 좌표
    float2 uvs[6] =
    {
        float2(0.0f, 0.0f),
        float2(1.0f, 0.0f),
        float2(0.0f, 1.0f),

        float2(0.0f, 1.0f),
        float2(1.0f, 0.0f),
        float2(1.0f, 1.0f)
    };

    output.position = float4(ndcPositions[vertexID], 0.0f, 1.0f);
    output.uv = uvs[vertexID];
    
    return output;
}
