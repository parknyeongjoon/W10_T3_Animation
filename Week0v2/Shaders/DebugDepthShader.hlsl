cbuffer CameraConstant : register(b0)
{
    matrix ViewMatrix;
    matrix ProjMatrix;
    matrix InvViewMatrix;
    matrix InvProjMatrix;
    
    float3 CameraPos;
    float NearPlane;
    float3 CameraForward;
    float FarPlane;
};

cbuffer ViewportInfo : register(b1)
{
    float2 ViewportSize;
    float2 ViewportOffset;
}

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
    float2 viewportUV = input.uv * ViewportSize + ViewportOffset;
    
    float depth = SceneDepthTex.Sample(PointSampler, viewportUV).r;
    if (depth == 1.0)
    {
        return float4(0, 0, 0, 1);
    }

    float linearDepth = (NearPlane * FarPlane) / (FarPlane - depth * (FarPlane - NearPlane));
    float normalized = saturate((linearDepth - NearPlane) / (FarPlane - NearPlane));

    float expo = 1 - exp(-normalized * 1.0); 


    return float4(expo, expo, expo, 1.0);
}
