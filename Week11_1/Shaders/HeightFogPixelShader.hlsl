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
    float FogMaxOpacity; // MaxOpacity 추가
    float LightShaftDensity; // 추가: 광선 밀도 [4]
}

Texture2D DepthTexture : register(t0);
Texture2D SceneTexture : register(t1);
SamplerState SamplerLinear : register(s0);

// Screen space to World space
float3 ReconstructWorldPos(float2 uv)
{
    // 1. 클립 공간 좌표 계산
    float4 clipPos = float4(uv * 2.0 - 1.0, 1.0, 1.0);

    clipPos.y *= -1.0;
    
    // 2. 뷰/프로젝션 행렬 역변환 (엔진별 행렬 전달 필요)
    float4 worldPos = mul(clipPos, InvProjMatrix);
    worldPos /= worldPos.w;
    worldPos = mul(worldPos, InvViewMatrix);
    
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
    float3 worldPosition = ReconstructWorldPos(input.uv);
    
    float dist = distance(CameraPos, worldPosition);
    //거리기반 감쇠
    float fogRange = (FogEnd - FogStart) * 0.5;
    float disFactor = saturate((dist - FogStart) / fogRange);

    float fogFactor = disFactor;
        
    if (bIsHeightFog)
    {
        float FogHeight = FogZPosition + FogBaseHeight;
        
        // 높이 기반 (지수 감쇠)
        float heightDiff = (worldPosition.z + CameraPos.z) - FogHeight;
        float heightFactor = saturate(exp(-(heightDiff) * HeightFallOff));
        fogFactor = saturate((disFactor + heightFactor)/2.0); //factor가 클수록 fogcolor에 가까워짐
    }
    
    float FinalFogFactor = min(saturate(fogFactor * FogDensity), FogMaxOpacity);

    float transitionStart = FogStart;
    float transitionEnd = FogStart * 3.0f;
    float transitionWeight = saturate((dist - transitionStart) / (transitionEnd - transitionStart));
    
    float outputFogFactor = lerp(0.0f, FinalFogFactor, transitionWeight);
    
    float3 FinalColor = lerp(sceneColor.rgb, FogColor, outputFogFactor);
    
    if (depth == 1) //배경(오브젝트가 아닐때)
    {
        FinalColor = lerp(sceneColor.rgb, FogColor, min(FogDensity, FogMaxOpacity));
    }

    return float4(FinalColor, 1.0);
}
