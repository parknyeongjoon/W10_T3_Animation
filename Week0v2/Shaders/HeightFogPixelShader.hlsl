cbuffer CameraConstant : register(b0)
{
    matrix ViewMatrix;
    matrix ProjMatrix;
    matrix ViewProjMatrix;
    
    float3 CameraPos;
    float NearPlane;
    float3 CameraForward;
    float FarPlane;
};

cbuffer FogParams : register(b6)
{
    float FogDensity;
    float FogHeightFalloff;
    float HeightOffset;
    float StartDistance;
    
    float4 InscatteringColor;
    float4 DirectionalInscatteringColor;
    
    float3 DirectionalLightDirection;
    float FogCutoffDistance;
    
    float MaxOpacity;
    float DirectionalInscatteringExponent;
    float DirectionalInscatteringStartDistance;
}

Texture2D SceneTexture : register(t5);
Texture2D DepthTexture : register(t0);
SamplerState SamplerLinear : register(s0);

// Screen space to World space
float3 ReconstructWorldPosition(float2 UV, float Depth)
{
    float linearDepth = (NearPlane * FarPlane) / (FarPlane - Depth * (FarPlane - NearPlane));
    
    // 1. 클립 공간 좌표 계산 (NDC)
    float4 clipPos = float4(UV * 2.0f - 1.0f, Depth, 1.0f);
    clipPos.y = -clipPos.y;
    
    // 2. 투영 행렬의 역행렬 계산 - 수정된 버전
    float4x4 invProj;
    invProj._11 = 1.0f / ProjMatrix._11;
    invProj._12 = 0.0f;
    invProj._13 = 0.0f;
    invProj._14 = 0.0f;
    
    invProj._21 = 0.0f;
    invProj._22 = 1.0f / ProjMatrix._22;
    invProj._23 = 0.0f;
    invProj._24 = 0.0f;
    
    invProj._31 = 0.0f;
    invProj._32 = 0.0f;
    invProj._33 = 0.0f;
    invProj._34 = 1.0f;
    
    invProj._41 = 0.0f;
    invProj._42 = 0.0f;
    invProj._43 = -1.0f;
    invProj._44 = (1.0f / ProjMatrix._43);
    
    // 3. 뷰 공간으로 변환
    float4 viewPos = mul(invProj, clipPos);
    viewPos /= viewPos.w;
    
    // 선형 깊이를 사용하여 뷰 공간 위치 조정
    viewPos.z = -linearDepth;
    
    // 4. 뷰 행렬의 역행렬 계산
    float4x4 invView;
    
    // 회전 부분 (3x3 부분행렬의 전치)
    invView._11 = ViewMatrix._11;
    invView._12 = ViewMatrix._21;
    invView._13 = ViewMatrix._31;
    invView._14 = 0.0f;
    
    invView._21 = ViewMatrix._12;
    invView._22 = ViewMatrix._22;
    invView._23 = ViewMatrix._32;
    invView._24 = 0.0f;
    
    invView._31 = ViewMatrix._13;
    invView._32 = ViewMatrix._23;
    invView._33 = ViewMatrix._33;
    invView._34 = 0.0f;

    invView._41 = CameraPos.x;
    invView._42 = CameraPos.y;
    invView._43 = CameraPos.z;
    invView._44 = 1.0f;

    float4 worldPos = mul(invView, float4(viewPos.xyz, 1.0f));
    
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
    float normalized = saturate((linearDepth - NearPlane) / (FarPlane - NearPlane));
    
    float3 worldPosition = ReconstructWorldPosition(input.uv, depth);
    float heightDifference = worldPosition.z - HeightOffset;
    
    // 높이 기반 안개 계산
    float heightTransition = smoothstep(0.0f, -20.0f, heightDifference);
    float heightFactor = lerp(1.0f, exp(FogHeightFalloff * min(0.0f, heightDifference) * 3.0f), heightTransition);
    
    // 정규화된 깊이 값을 사용하여 거리 기반 안개 계산 (from debug depth shader)
    float distanceFactor = 1.0 - exp(-normalized * 5.0);
    
    float fogFactor = FogDensity * heightFactor * distanceFactor;
    float3 fogColor = InscatteringColor.rgb;
    
    fogFactor = min(fogFactor, MaxOpacity);
    
    return float4(lerp(sceneColor.rgb, fogColor, fogFactor), sceneColor.a);
}
