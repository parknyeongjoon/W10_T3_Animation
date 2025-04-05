cbuffer DepthToWorldConstant : register(b0)
{
    matrix InvView;
    matrix InvProj;
    float nearPlane;
    float farPlane;
};

Texture2D<float> SceneDepthTex : register(t0);
SamplerState PointSampler : register(s0);

struct VS_OUT
{
    float4 position : SV_POSITION; // 변환된 화면 좌표
    float2 uv : TEXCOORD0; // UV 좌표
};

VS_OUT mainVS(uint id : SV_VertexID)
{
    VS_OUT output;
    
    // 풀스크린 쿼드 정점 위치 (Triangle Strip 용)
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
    
    // 수정된 position 계산 (z=0, w=1 중요!)
    output.position = float4(pos[id], 0.0, 1.0);
    output.uv = uv[id];
    
    return output;
}

float4 mainPS(VS_OUT input) : SV_Target
{
    float depth = SceneDepthTex.Sample(PointSampler, input.uv).r;
    if (depth == 1.0)
    {
        return float4(0, 0, 0, 1);
    }

    float ndcx = input.uv.x * 2.0 - 1.0;
    float ndcy = 1.0f - input.uv.y * 2.0;
    float linearDepth = (nearPlane * farPlane) / (farPlane - depth * (farPlane - nearPlane));
    float normalized = saturate((linearDepth - nearPlane) / (farPlane - nearPlane));
    
    float4 ndcPosition = float4(ndcx, ndcy, depth, 1.0);
    float4 viewPosition = mul(ndcPosition, InvProj);
    viewPosition /= viewPosition.w;
    float4 worldPosition = mul(viewPosition, InvView);
    worldPosition /= worldPosition.w;
    return worldPosition;
    // 감마 보정 적용
    //float gammaCorrected = pow(normalized, 1.0 / 4.0);
    // float expo = 1 - exp(-normalized * 5.0); // 5.0은 조정 가능한 falloff 계수


    // return float4(expo, expo, expo, 1.0);
}
