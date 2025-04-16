cbuffer FFogCameraConstant : register(b0)
{
    matrix InvProjMatrix;
    matrix InvViewMatrix;
    
    float3 CameraPos;
    float NearPlane;
    float3 CameraForward;
    float FarPlane;
};

cbuffer FViewportInfo : register(b1)
{
    float2 ViewportSize;
    float2 ViewportOffset;
}

cbuffer FFogParams : register(b2)
{
    float3 FogColor;
    float FogDensity;
    float FogStart;
    float FogEnd;
    float FogZPosition;
    float FogBaseHeight;
    float HeightFallOff;
    int bIsHeightFog;
    float ScatteringIntensity; // 추가: 빛 산란 강도 [4]
    float LightShaftDensity; // 추가: 광선 밀도 [4]
}

Texture2D DepthTexture : register(t0);
Texture2D SceneTexture : register(t1);
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
    float2 viewportUV = input.uv * ViewportSize + ViewportOffset;
    float4 sceneColor = SceneTexture.Sample(SamplerLinear, viewportUV);
    float depth = DepthTexture.Sample(SamplerLinear, viewportUV).r;
    
    float linearDepth = (NearPlane * FarPlane) / (FarPlane - depth * (FarPlane - NearPlane));
    float3 worldPosition = ReconstructWorldPosition(input.uv, depth);
    
    float dist = distance(CameraPos, worldPosition);
    //거리기반 감쇠
    float fogRange = FogEnd - FogStart;
    float disFactor = saturate((dist - FogStart) / fogRange);

    float fogFactor = disFactor;
        
    if (bIsHeightFog)
    {
        float FogHeight = FogZPosition + FogBaseHeight;
        
            // 높이 기반 (지수 감쇠)
        float heightDiff = worldPosition.z - FogHeight;
        float heightFactor = saturate(exp(-heightDiff * HeightFallOff)); // 0~1
        fogFactor = fogFactor * heightFactor; //factor가 클수록 fogcolor에 가까워짐
    }

    float3 FinalColor = lerp(sceneColor.rgb, FogColor, fogFactor * FogDensity);

    return float4(FinalColor, 1.0);
}
