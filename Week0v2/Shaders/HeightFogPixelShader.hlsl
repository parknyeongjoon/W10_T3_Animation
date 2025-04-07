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

cbuffer FogParams : register(b6)
{
    float FogDensity;
    float HeightFogStart;
    float HeightFogEnd;
    float MaxOpacity;

    float DistanceFogNear;
    float DistanceFotFar;
    float pad1, pad2;
    
    float4 InscatteringColor;
    float4 DirectionalInscatteringColor;
    
    float3 DirectionalLightDirection;
    float DirectionalInscatteringExponent;
    
    float DirectionalInscatteringStartDistance;
    float pad3, pad4, pad5;
    
    int IsExponential;
}

Texture2D SceneTexture : register(t5);
Texture2D DepthTexture : register(t0);
SamplerState SamplerLinear : register(s0);

// Screen space to World space
float3 ReconstructWorldPosition(float2 UV, float Depth)
{
    float ndcx = UV.x * 2.0 - 1.0;
    float ndcy = 1.0f - UV.y * 2.0;
    float4 clipPos = float4(ndcx, ndcy, Depth, 1.0f);
    
    float4 viewPos = mul(clipPos, InvProjMatrix);
    viewPos /= viewPos.w;
    
    float4 worldPos = mul(viewPos, InvViewMatrix);
    worldPos /= worldPos.w;
    return worldPos.xyz;
}

struct VS_OUT
{
    float4 position : SV_POSITION; // 변환된 화면 좌표
    float2 uv : TEXCOORD0; // UV 좌표
};

float4 mainPS(VS_OUT input) : SV_TARGET
{
    float4 sceneColor = SceneTexture.Sample(SamplerLinear, input.uv);
    float depth = DepthTexture.Sample(SamplerLinear, input.uv).r;
    
    float linearDepth = (NearPlane * FarPlane) / (FarPlane - depth * (FarPlane - NearPlane));
    float normalized = saturate((linearDepth - DistanceFogNear) / (DistanceFotFar - DistanceFogNear));
    float3 worldPosition = ReconstructWorldPosition(input.uv, depth);
    
    // 높이 기반 안개 계산
    float heightDiff = saturate((HeightFogEnd - worldPosition.z) / (HeightFogEnd - HeightFogStart));
    float heightFactor = saturate(1.f - exp(-heightDiff * 3.0f));
    
    // 정규화된 깊이 값을 사용하여 거리 기반 안개 계산 (from debug depth shader)
    float distanceFactor;
    
    if (IsExponential != 0)
    {
        distanceFactor = saturate(1.f - exp(-normalized * 5.0));
    }
    else
    {
        distanceFactor = normalized;
    }
    
    float fogFactor = FogDensity * heightFactor * distanceFactor;
    float3 fogColor = InscatteringColor.rgb;
    
    fogFactor = min(fogFactor, MaxOpacity);
    
    return float4(lerp(sceneColor.rgb, fogColor, fogFactor), sceneColor.a);
}
