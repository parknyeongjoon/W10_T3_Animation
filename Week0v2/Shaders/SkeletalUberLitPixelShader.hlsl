#include "ShaderHeaders/GSamplers.hlsli"

Texture2D DiffuseTexture : register(t0);
Texture2D AmbientTexture : register(t1);
Texture2D SpecularTexture : register(t2);
Texture2D BumpTexture : register(t3);
Texture2D AlphaTexture : register(t4);
Texture2D NormalTexture : register(t5);

// 샘플링 패턴
static const int filterSize = 1; // 중앙을 포함한 반경 EX. (-2, -1, 0, 1, 2) -> 5x5

// StructuredBuffer<uint> TileLightIndices : register(t2);

static const int CASCADE_COUNT = 4;

//Texture2D SpotLightShadowMap[8] : register(t3);
Texture2D SpotLightAtlas : register(t6);
Texture2D DirectionalLightShadowMap[CASCADE_COUNT] : register(t11);

TextureCubeArray<float> PointLightShadowMapArray : register(t7);
//TextureCube<float> PointLightShadowMap[8] : register(t15);

Texture2DArray<float2> PointLightVSM[8] : register(t23);

#define MAX_POINTLIGHT_COUNT 8

cbuffer FMaterialConstants : register(b0)
{
    float3 DiffuseColor;
    float TransparencyScalar;
    float3 MatAmbientColor;
    float DensityScalar;
    float3 SpecularColor;
    float SpecularScalar;
    float3 EmissiveColor;
    float pad;
}

cbuffer FConstatntBufferActor : register(b1)
{
    float4 UUID; // 임시
    uint IsSelectedActor;
    float3 padding;
}

struct FDirectionalLight
{
    float3 Direction;
    float Intensity;
    float4 Color;

    float CascadeSplit0;
    float CascadeSplit1;
    float CascadeSplit2;
    float CascadeSplit3;

    row_major float4x4 View[CASCADE_COUNT];
    row_major float4x4 Projection[CASCADE_COUNT];
    
    uint CastShadow;
    float3 Pad;
};

struct FPointLight
{
    float3 Position;
    float Radius;
    
    float4 Color;
    
    float Intensity;
    float AttenuationFalloff;
    uint CastShadow;
    float pad;
    
    row_major float4x4 View[6];
    row_major float4x4 Proj;
};

struct FSpotLight
{
    float3 Position;
    float Intensity;
    
    float4 Color;
    
    float3 Direction;
    float InnerAngle;
    
    float OuterAngle;
    uint CastShadow;
    float2 pad;
    
    row_major float4x4 View;
    row_major float4x4 Proj;
    
    float4 AtlasUV;
};


cbuffer FLightingConstants : register(b2)
{
    uint NumPointLights;
    uint NumSpotLights;
    float2 pad2;

    FDirectionalLight DirLight;
    FPointLight PointLights[8];
    FSpotLight SpotLights[8];
};

cbuffer FFlagConstants : register(b3)
{
    uint IsLit;
    uint IsNormal;
    uint IsVSM;
    uint IsGPUSkinning;
}

cbuffer FSubUVConstant : register(b4)
{
    float indexU;
    float indexV;
    float2 subUVpadding;
}

cbuffer FCameraConstant : register(b5)
{
    row_major matrix ViewMatrix;
    row_major matrix ProjMatrix;
    row_major matrix ViewProjMatrix;
    
    float3 CameraPos;
    float NearPlane;
    float3 CameraForward;
    float FarPlane;
};

cbuffer FComputeConstants : register(b6){
    row_major matrix InverseView;
    row_major matrix InverseProj;
    float screenWidth;
    float screenHeight;
    int numTilesX;
    int numTilesY;
}

struct PS_INPUT
{
    float4 position : SV_POSITION; // 변환된 화면 좌표
    float3 worldPos : POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : TEXCOORD1;
    float3x3 TBN: TEXCOORD3;
};

struct PS_OUTPUT
{
    float4 color : SV_Target0;
};

float CalculatePointLightShadow(float3 worldPos,float3 worldNormal, FPointLight PointLight,int mapIndex)
{
    float4 WoldPos4 = float4(worldPos, 1.0f);
  
    float3 LightDirection = normalize(worldPos - PointLight.Position);
  
    float3 LightDirectionAbs = abs(LightDirection); // 절대값 각 축 크기 구함.
    int face;
    if (LightDirectionAbs.x >= LightDirectionAbs.y && LightDirectionAbs.x >= LightDirectionAbs.z) 
        face = LightDirection.x > 0 ? 0 : 1; // +X, -X
    else if (LightDirectionAbs.y >= LightDirectionAbs.z)
        face = LightDirection.y > 0 ? 2 : 3; // +Y, -Y
    else
        face = LightDirection.z > 0 ? 4 : 5; // +Z, -Z
  
    float4 LightViewPos = mul(WoldPos4, PointLight.View[face]); // 월드 → 라이트(큐브 face) 공간
    float4 clipPos = mul(LightViewPos, PointLight.Proj); // 라이트 공간 -> Clip space

  //FIXME : bias 적용
  // NDC 깊이 (0~1) 추출
  //float refDepth = clipPos.z / clipPos.w - bias;
    float bias = max(0.001 * (1.0 - dot(worldNormal, LightDirection)), 0.0001);
    float refDepth = clipPos.z / clipPos.w - bias;

   // PCF 파라미터 설정
    float shadowSum = 0.0;
    float numSamples = 0.0;
    static const float pcfRadius = 0.001; // PCF 필터 반경 (조정 가능)
  
  // 샘플링 패턴 (5x5 필터링)
    [unroll]
    for (int x = -filterSize; x <= filterSize; x++)
    {
        [unroll]
        for (int y = -filterSize; y <= filterSize; y++)
        {
          // 샘플링 오프셋 벡터 계산
            float3 offset = float3(0, 0, 0);
            if (face == 0 || face == 1) // X축 면
                offset = float3(0, x * pcfRadius, y * pcfRadius);
            else if (face == 2 || face == 3) // Y축 면
                offset = float3(x * pcfRadius, 0, y * pcfRadius);
            else // Z축 면
                offset = float3(x * pcfRadius, y * pcfRadius, 0);
          
          // 오프셋을 적용한 샘플링 방향
            float3 sampleDirection = normalize(LightDirection + offset);
          
          // 샘플링
            float shadowSample = PointLightShadowMapArray.SampleCmp(
              CompareSampler,
              float4(sampleDirection, mapIndex),
              refDepth
          );
          
            shadowSum += shadowSample;
            numSamples += 1.0;
        }
    }
  
    return shadowSum / numSamples;
}

float3 CalculateVSMPointLightShadow(float3 WorldPos, float3 Normal, FPointLight Light, Texture2DArray<float2> ShadowMap)
{
    float3 ToLight = WorldPos - Light.Position;
    float3 dir = normalize(ToLight);
    float dist = length(ToLight);

    uint face = 0;
    float3 absDir = abs(dir);
    if (absDir.x > absDir.y && absDir.x > absDir.z)
        face = (dir.x > 0) ? 0 : 1;
    else if (absDir.y > absDir.z)
        face = (dir.y > 0) ? 2 : 3;
    else
        face = (dir.z > 0) ? 4 : 5;
    
    face = 1;
    float4 LightSpacePos = mul(float4(WorldPos, 1.0), Light.View[face]);
    LightSpacePos = mul(LightSpacePos, Light.Proj);
    
    float2 uv = LightSpacePos.xy / LightSpacePos.w * 0.5 + 0.5;
    uv.y = 1.0 - uv.y;

    float worldDepth = LightSpacePos.z / LightSpacePos.w;

    if (uv.x < 0 || uv.x > 1 || uv.y < 0 || uv.y > 1 || worldDepth < 0 || worldDepth > 1)
        return 1.0;
    
    float bias = max(0.0001 * (1.0 - dot(Normal, dir)), 0.00005);
    float distanceScale = saturate(1.0 - dist / Light.Radius);
    bias *= distanceScale;
    bias = max(bias, 0.00001);
    float2 moments = ShadowMap.Sample(linearSampler, float3(uv, face));
    float mean = moments.x;
    float meanSq = moments.y;
    float variance = meanSq - mean * mean;
    variance = max(variance, 0.00001);
        
    float d = worldDepth - mean;
    float pMax = variance / (variance + d * d);
    float distanceFactor = distanceScale * 100;
    pMax = pow(saturate(pMax), distanceFactor);
    return max(saturate(pMax), worldDepth <= mean + bias);
}

float3 CalculateDirectionalLight(  
    FDirectionalLight Light,  
    float3 Normal,  
    float3 ViewDir,  
    float3 Albedo,
    float2 UV)  
{  
    // 광원 방향  
    float3 LightDir = normalize(-Light.Direction);  

    // 디퓨즈 계산  
    float NdotL = max(dot(Normal, LightDir), 0.0);  
    float3 Diffuse = Light.Color.rgb * Albedo * NdotL;  

    // 스페큘러 (Blinn-Phong)  
    float3 HalfVec = normalize(LightDir + ViewDir);  
    float NdotH = max(dot(Normal, HalfVec), 0.0);  
    float Specular = pow(NdotH, SpecularScalar * 128.0) * SpecularScalar;
    float3 SpecMap   = SpecularTexture.Sample(linearSampler, UV).rgb;
    float3 specularColor = Light.Color.rgb * Specular * SpecularColor * SpecMap;  

    // 최종 광원 영향  
    return (Diffuse + specularColor) * Light.Intensity;  
}

float3 CalculatePointLight(  
    FPointLight Light,  
    float3 WorldPos,  
    float3 Normal,  
    float3 ViewDir,  
    float3 Albedo,
    float2 UV)  
{  
    // 광원 거리/방향  
    float3 LightDir = Light.Position - WorldPos;
    float Distance = length(LightDir);  
    LightDir = normalize(LightDir);  

    // 반경 체크  
    if(Distance > Light.Radius) return float3(0,0,0);  

    // 감쇠 계산  
    float Attenuation = Light.Intensity / (1.0 + Light.AttenuationFalloff * Distance * Distance);  
    Attenuation *= 1.0 - smoothstep(0.0, Light.Radius, Distance);  

    // 디퓨즈  
    float NdotL = max(dot(Normal, LightDir), 0.0);
    float3 Diffuse = Light.Color.rgb * Albedo * NdotL;
    //return float3(abs(LightDir));
    //return float3(NdotL.xxx);

#if LIGHTING_MODEL_LAMBERT
    return Diffuse * Light.Intensity * Attenuation;
#endif
    // 스페큘러  
    float3 HalfVec = normalize(LightDir + ViewDir);  
    float NdotH = max(dot(Normal, HalfVec), 0.0);  
    float Specular = pow(NdotH, SpecularScalar * 128.0) * SpecularScalar;
    float3 SpecMap   = SpecularTexture.Sample(linearSampler, UV).rgb;
    float3 specularColor = Light.Color.rgb * Specular * SpecularColor * SpecMap;

    return (Diffuse + specularColor) * Light.Intensity * Attenuation;  
}

float3 CalculateSpotLight(
    FSpotLight Light,
    float3 WorldPos,
    float3 Normal,
    float3 ViewDir,
    float3 Albedo,
    float2 UV)
{
    // 조명 방향 벡터 (광원 위치 → 픽셀)
    float3 LightDir = normalize(Light.Position - WorldPos);
    float Distance = length(Light.Position - WorldPos);

    // Spot Light 중심 방향
    float3 SpotDirection = normalize(-Light.Direction);

    // 각도 감쇠 계산
    float CosInner = cos(Light.InnerAngle);
    float CosOuter = cos(Light.OuterAngle);
    float CosAngle = dot(SpotDirection, LightDir); // 광원 기준 → 픽셀 방향

    if (CosAngle < CosOuter)
        return float3(0, 0, 0);

    float SpotAttenuation = saturate((CosAngle - CosOuter) / (CosInner - CosOuter));
    float DistanceAttenuation = (1.0 / (1.0 + Distance * Distance * 0.01)); // 간단 거리 감쇠

    // 디퓨즈
    float NdotL = max(dot(Normal, SpotDirection), 0.0);
    float3 Diffuse = Light.Color.rgb * Albedo * NdotL;

#if LIGHTING_MODEL_LAMBERT
    return Diffuse * Light.Intensity * SpotAttenuation * DistanceAttenuation;
#endif
    // 스페큘러 (Blinn-Phong)
    float3 HalfVec = normalize(SpotDirection + ViewDir);
    float NdotH = max(dot(Normal, HalfVec), 0.0);
    float Specular = pow(NdotH, SpecularScalar * 128.0) * SpecularScalar;
    float3 SpecMap   = SpecularTexture.Sample(linearSampler, UV).rgb;
    float3 specularColor = Light.Color.rgb * Specular * SpecularColor * SpecMap;

    return (Diffuse + specularColor) * Light.Intensity * SpotAttenuation * DistanceAttenuation;
}

// 타일 크기 설정
static const uint TILE_SIZE_X = 16;
static const uint TILE_SIZE_Y = 16;

float4 WorldToLight(float3 WorldPos, row_major float4x4 View, row_major float4x4 Proj)
{
    float4 ViewPos = mul(float4(WorldPos, 1.0), View);
    float4 LightViewPos = mul(ViewPos, Proj);
    return LightViewPos;
}

float CalculateShadow(float3 WorldPos, float3 LightPos, float3 Normal, float3 LightDir, float4x4 View, float4x4 Projection, Texture2D ShadowMap)
{
    float shadow = 0;
    float4 LightViewPos = WorldToLight(WorldPos, View, Projection);
    float2 shadowUV = LightViewPos.xy / LightViewPos.w * 0.5 + 0.5;
    shadowUV.y = 1.0 - shadowUV.y;
    float worldDepth = LightViewPos.z / LightViewPos.w;

    if (shadowUV.x >= 0 && shadowUV.x <= 1 &&
        shadowUV.y >= 0 && shadowUV.y <= 1 &&
        worldDepth >= 0 && worldDepth <= 1)
    {
        float bias = max(0.01 * (1.0 - dot(Normal, -LightDir)), 0.005);
        bias = max(bias, 0.005);
        
        int numSamples = 0.0;
        static uint textureWidth, textureHeight;
        ShadowMap.GetDimensions(textureWidth, textureHeight);
        
        [unroll]
        for (int x = -filterSize; x <= filterSize; ++x)
        {
            [unroll]
            for (int y = -filterSize; y <= filterSize; ++y)
            {
                float2 texelSize = 1.0 / float2(textureWidth, textureHeight);
                float2 offset = float2(x, y) * texelSize;
                float sample = ShadowMap.Sample(pointSampler, shadowUV + offset).r;
                shadow += (worldDepth >= sample + bias) ? 1.0 : 0.0;
                numSamples += 1;
            }
        }
        shadow = shadow / numSamples;
    }
    return shadow;
}

float CalculateSpotLightShadowAtlas(FSpotLight SpotLight, float3 LightPos, float3 WorldPos, float3 Normal, float3 LightDir, float4x4 View, float4x4 Projection)
{
    float shadow = 0;
    float4 LightViewPos = WorldToLight(WorldPos, View, Projection);
    
    float2 shadowUV = LightViewPos.xy / LightViewPos.w * 0.5 + 0.5;
    shadowUV.y = 1.0 - shadowUV.y;
    float worldDepth = LightViewPos.z / LightViewPos.w;
    
    float2 atlasUV = shadowUV * SpotLight.AtlasUV.zw + SpotLight.AtlasUV.xy;
    
    // 경계 검사
    if (atlasUV.x >= SpotLight.AtlasUV.x &&
       atlasUV.x <= (SpotLight.AtlasUV.x + SpotLight.AtlasUV.z) &&
       atlasUV.y >= SpotLight.AtlasUV.y &&
       atlasUV.y <= (SpotLight.AtlasUV.y + SpotLight.AtlasUV.w) &&
       worldDepth >= 0 && worldDepth <= 1)
    {
        float bias = max(0.0001 * (1.0 - dot(Normal, -LightDir)), 0.00005);
        float dist = length(WorldPos - LightPos);
        float distanceScale = saturate(1.0 - dist / 100.0f);
        bias *= distanceScale;
        bias = max(bias, 0.00005);
        
        static const float texelSize = 1.0 / 1024.0; // 1024x1024 해상도 가정
        
        [unroll]
        for (int x = -filterSize; x <= filterSize; ++x)
        {
            [unroll]
            for (int y = -filterSize; y <= filterSize; ++y)
            {
                float2 offset = float2(x, y) * texelSize;
                float sampleDepth = SpotLightAtlas.Sample(
                    pointSampler,
                    atlasUV + offset
                ).r;
                
                shadow += (worldDepth >= sampleDepth + bias) ? 1.0 : 0.0;
            }
        }
        shadow /= 9.0; // 3x3 PCF 평균
    }
    
    return shadow;
}

float CalculateSpotLightVSMShadowAtlas(FSpotLight SpotLight, float3 LightPos, float3 WorldPos, float3 Normal, float3 LightDir, float4x4 View, float4x4 Projection)
{
    float shadow = 0;
    float4 LightViewPos = WorldToLight(WorldPos, View, Projection);
    
    float2 shadowUV = LightViewPos.xy / LightViewPos.w * 0.5 + 0.5;
    shadowUV.y = 1.0 - shadowUV.y;
    float worldDepth = LightViewPos.z / LightViewPos.w;
    
    float2 atlasUV = shadowUV * SpotLight.AtlasUV.zw + SpotLight.AtlasUV.xy;
    
    // 경계 검사
    if (atlasUV.x >= SpotLight.AtlasUV.x &&
       atlasUV.x <= (SpotLight.AtlasUV.x + SpotLight.AtlasUV.z) &&
       atlasUV.y >= SpotLight.AtlasUV.y &&
       atlasUV.y <= (SpotLight.AtlasUV.y + SpotLight.AtlasUV.w) &&
       worldDepth >= 0 && worldDepth <= 1)
    {
        float bias = max(0.0001 * (1.0 - dot(Normal, -LightDir)), 0.00005);
        float dist = length(WorldPos - LightPos);
        float distanceScale = saturate(1.0 - dist / 100.0f);
        bias *= distanceScale;
        bias = max(bias, 0.00005);
        float2 moments = SpotLightAtlas.Sample(linearSampler, atlasUV).rg;
        float mean = moments.x;
        float meanSq = moments.y;
        float variance = meanSq - mean * mean;
        variance = max(variance, 0.00001);
        
        float d = worldDepth - mean;
        float pMax = variance / (variance + d * d);
        float distanceFactor = distanceScale * 50;
        pMax = pow(saturate(pMax), distanceFactor);
        return max(saturate(pMax), worldDepth <= mean + bias);
    }
    return 0;
}

// 다음 캐스케이드 레벨로의 부드러운 전환을 위한 블렌딩 함수
float CalculateBlendedShadow(float3 WorldPos, float3 Normal, float3 LightDir, 
                            float4x4 View1, float4x4 Proj1, Texture2D ShadowMap1,
                            float4x4 View2, float4x4 Proj2, Texture2D ShadowMap2,
                            float blendFactor)
{
    float shadow1 = CalculateShadow(WorldPos, float3(0, 0, 0) - LightDir,
    Normal, LightDir, View1, Proj1, ShadowMap1);
    float shadow2 = CalculateShadow(WorldPos, float3(0, 0, 0) - LightDir, 
    Normal, LightDir, View2, Proj2, ShadowMap2);
    
    // 선형 보간을 사용하여 두 그림자 값 블렌딩
    return lerp(shadow1, shadow2, blendFactor);
}

// 메인 디렉셔널 그림자 계산 함수
float CalculateDirectionalShadow(float3 WorldPos, float3 Normal)
{
    // 뷰 공간에서의 깊이 계산
    float4 WorldViewPos = mul(float4(WorldPos, 1.0f), ViewMatrix);
    float nonLinearDepth = WorldViewPos.z;
    
    // 블렌딩 영역의 크기 (각 캐스케이드 분할점의 % 비율)
    const float BLEND_RATIO = 0.1; // 조정 가능한 값
    
    // 각 캐스케이드 레벨의 블렌딩 영역 계산
    float blend0 = BLEND_RATIO * DirLight.CascadeSplit0;
    float blend1 = BLEND_RATIO * DirLight.CascadeSplit1;
    float blend2 = BLEND_RATIO * DirLight.CascadeSplit2;
    
    // 캐스케이드 레벨 0 (가장 가까운 레벨)
    if (nonLinearDepth < DirLight.CascadeSplit0 - blend0)
    {
        return CalculateShadow(WorldPos, float3(0, 0, 0) - DirLight.Direction, Normal, DirLight.Direction,
                            DirLight.View[0], DirLight.Projection[0], 
                            DirectionalLightShadowMap[0]);
    }
    // 캐스케이드 레벨 0과 1 사이의 블렌딩 영역
    if (nonLinearDepth < DirLight.CascadeSplit0 + blend0)
    {
        float blendFactor = (nonLinearDepth - (DirLight.CascadeSplit0 - blend0)) / (2 * blend0);
        return CalculateBlendedShadow(WorldPos, Normal, DirLight.Direction,
                                    DirLight.View[0], DirLight.Projection[0], DirectionalLightShadowMap[0],
                                    DirLight.View[1], DirLight.Projection[1], DirectionalLightShadowMap[1],
                                    blendFactor);
    }
    // 캐스케이드 레벨 1
    if (nonLinearDepth < DirLight.CascadeSplit1 - blend1)
    {
        return CalculateShadow(WorldPos, float3(0, 0, 0) - DirLight.Direction, 
                            Normal, DirLight.Direction,
                            DirLight.View[1], DirLight.Projection[1], 
                            DirectionalLightShadowMap[1]);
    }
    // 캐스케이드 레벨 1과 2 사이의 블렌딩 영역
    if (nonLinearDepth < DirLight.CascadeSplit1 + blend1)
    {
        float blendFactor = (nonLinearDepth - (DirLight.CascadeSplit1 - blend1)) / (2 * blend1);
        return CalculateBlendedShadow(WorldPos, Normal, DirLight.Direction,
                                    DirLight.View[1], DirLight.Projection[1], DirectionalLightShadowMap[1],
                                    DirLight.View[2], DirLight.Projection[2], DirectionalLightShadowMap[2],
                                    blendFactor);
    }
    // 캐스케이드 레벨 2
    if (nonLinearDepth < DirLight.CascadeSplit2 - blend2)
    {
        return CalculateShadow(WorldPos, float3(0, 0, 0) - DirLight.Direction, Normal, DirLight.Direction,
                            DirLight.View[2], DirLight.Projection[2], 
                            DirectionalLightShadowMap[2]);
    }
    // 캐스케이드 레벨 2와 3 사이의 블렌딩 영역
    if (nonLinearDepth < DirLight.CascadeSplit2 + blend2)
    {
        float blendFactor = (nonLinearDepth - (DirLight.CascadeSplit2 - blend2)) / (2 * blend2);
        return CalculateBlendedShadow(WorldPos, Normal, DirLight.Direction,
                                    DirLight.View[2], DirLight.Projection[2], DirectionalLightShadowMap[2],
                                    DirLight.View[3], DirLight.Projection[3], DirectionalLightShadowMap[3],
                                    blendFactor);
    }
    // 캐스케이드 레벨 3 (가장 먼 레벨)
    return CalculateShadow(WorldPos, float3(0, 0, 0) - DirLight.Direction, Normal, DirLight.Direction,
                        DirLight.View[3], DirLight.Projection[3], 
                        DirectionalLightShadowMap[3]);
}

float CalculateVSMShadow(float3 WorldPos, float3 LightPos, float3 Normal, float3 LightDir, float4x4 View, float4x4 Projection, Texture2D ShadowMap)
{
    float shadow = 0;
    float4 LightViewPos = WorldToLight(WorldPos, View, Projection);
            
    float2 shadowUV = LightViewPos.xy / LightViewPos.w * 0.5 + 0.5;
    shadowUV.y = 1.0 - shadowUV.y;
    float worldDepth = LightViewPos.z / LightViewPos.w;

    if (shadowUV.x >= 0 && shadowUV.x <= 1 &&
        shadowUV.y >= 0 && shadowUV.y <= 1 &&
        worldDepth >= 0 && worldDepth <= 1)
    {
        float bias = max(0.0001 * (1.0 - dot(Normal, -LightDir)), 0.00005);
        float dist = length(WorldPos - LightPos);
        float distanceScale = saturate(1.0 - dist / 100.0f);
        bias *= distanceScale;
        bias = max(bias, 0.00001);
        float2 moments = ShadowMap.Sample(linearSampler, shadowUV);
        float mean = moments.x;
        float meanSq = moments.y;
        float variance = meanSq - mean * mean;
        variance = max(variance, 0.00001);
        
        float d = worldDepth - mean;
        float pMax = variance / (variance + d * d);
        float distanceFactor = distanceScale * 100;
        pMax = pow(saturate(pMax), distanceFactor);
        return max(saturate(pMax), worldDepth <= mean + bias);
    }
    return 0;
}

PS_OUTPUT mainPS(PS_INPUT input)
{
    PS_OUTPUT output;
    float2 uv = input.texcoord;

    float4 baseColor = DiffuseTexture.Sample(linearSampler, uv) * float4(DiffuseColor, 1.0);  
    if (!IsLit && !IsNormal)
    {
        output.color = float4(baseColor.rgb, 1.0);
        return output;
    }
    
#if LIGHTING_MODEL_GOURAUD
    if (IsSelectedActor == 1)
        input.color *= 5;

    output.color = float4(baseColor.rgb * input.color.rgb, 1.0);
    return output;
#endif
    // Bump (Height)
    float height = BumpTexture.Sample(linearSampler, uv).r;
    float scale = 0.05;
    float2 viewDir = normalize((CameraPos - input.worldPos).xy);
    float2 pUV = uv + viewDir * ((height - 0.5) * scale);
    
    // Normal
    float4 normalTex = ((NormalTexture.Sample(linearSampler, uv)- 0.5) * 2);

    float2 tileSize = float2(TILE_SIZE_X, TILE_SIZE_Y);
    
    uint2 pixelCoord = uint2(input.position.xy);
    uint2 tileCoord = pixelCoord / tileSize; // 각 성분별 나눔
    uint tileIndex = tileCoord.x + tileCoord.y * numTilesX;
    
    float3 Normal = input.normal;
    Normal = normalize(mul(normalTex.rgb, input.TBN));
    if (length(Normal) < 0.001) // tangent 값이 없을때 ( uv 없을때 )
    {
        Normal = input.normal;
    } 
    
    if (IsNormal)
    {
        //Normal = input.normal;
        Normal = Normal * 0.5 + 0.5;
        output.color = float4(Normal.rgb, 1.0);
        return output;
    }
    
    float3 ViewDir = normalize(CameraPos - input.worldPos); // CameraPos도 안 들어오고, ViewDir은 카메라의 Foward 아닌가요?

    // Ambient
    float3 ambientTex = AmbientTexture.Sample(linearSampler, uv).rgb;
    float3 TotalLight = ambientTex * MatAmbientColor;
    // TODO : Lit이면 낮은 값 Unlit이면 float3(1.0f,1.0f,1.0f)면 됩니다.
    //float3 TotalLight = float3(0.0f, 0.0f, 0.0f); // 전역 앰비언트  

    // Selected
    if (IsSelectedActor == 1)
        TotalLight = TotalLight * 10.0f;
    TotalLight += EmissiveColor;

    // DirectionalLight
    float3 DirLightColor = CalculateDirectionalLight(DirLight, Normal, ViewDir, baseColor.rgb, uv);
    if (length(DirLightColor) > 0.0 && DirLight.CastShadow)
    {
        float dirShadow = CalculateShadow(input.worldPos, float3(0, 0, 0) - DirLight.Direction, Normal, DirLight.Direction, DirLight.View[0], DirLight.Projection[0], DirectionalLightShadowMap[0]);
        DirLightColor *= (1 - dirShadow);
    }
    TotalLight += DirLightColor;

    // PointLight
    for(uint j=0; j<NumPointLights; ++j)
    {
        //uint listIndex = tileIndex * MAX_POINTLIGHT_COUNT + j;
        //uint lightIndex = TileLightIndices[listIndex];
        //if (lightIndex == 0xFFFFFFFF)
        //{
        //    break;
        //}
        float Shadow = 1.0;
        float3 LightColor = CalculatePointLight(PointLights[j], input.worldPos, Normal, ViewDir, baseColor.rgb, uv);
        if (length(LightColor) > 0.0 && PointLights[j].CastShadow)
        {
            Shadow = CalculatePointLightShadow(input.worldPos, input.normal, PointLights[j], j);
        }
        
        TotalLight += LightColor * Shadow;
    }

    // SpotLight
    for (uint k = 0; k < NumSpotLights; ++k)
    {
        float3 SpotLightColor = CalculateSpotLight(SpotLights[k], input.worldPos, input.normal, ViewDir, baseColor.rgb, uv);
        if (length(SpotLightColor) > 0.0 && SpotLights[k].CastShadow)
        {
            if (IsVSM)
            {
                float SpotShadow = CalculateSpotLightVSMShadowAtlas(SpotLights[k], SpotLights[k].Position, input.worldPos,
                Normal, SpotLights[k].Direction, SpotLights[k].View, SpotLights[k].Proj);
                SpotLightColor *= (SpotShadow);
            }
            else
            {
                float SpotShadow = CalculateSpotLightShadowAtlas(SpotLights[k], SpotLights[k].Position, input.worldPos,
                Normal, SpotLights[k].Direction, SpotLights[k].View, SpotLights[k].Proj);
                SpotLightColor *= (1 - SpotShadow);
            }
        }
        TotalLight += SpotLightColor;
    }

    // Alpha
    float alphaMap = AlphaTexture.Sample(linearSampler, uv).r;
    float finalAlpha = alphaMap * TransparencyScalar;
    
    // Final Color
    float4 FinalColor = float4(
        TotalLight * baseColor.rgb,  // RGB
        finalAlpha                   // 알파
    );
    output.color = FinalColor;
    
    return output;
}