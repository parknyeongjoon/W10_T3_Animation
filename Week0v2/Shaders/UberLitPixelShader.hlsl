#include "ShaderHeaders/GSamplers.hlsli"

#define NUM_POINT_LIGHT 4
#define NUM_SPOT_LIGHT 4

Texture2D Texture : register(t0);
Texture2D NormalTexture : register(t1);
StructuredBuffer<uint> TileLightIndices : register(t2);

cbuffer FMaterialConstants : register(b0)
{
    float3 DiffuseColor;
    float TransparencyScalar;
    float3 MatAmbientColor;
    float DensityScalar;
    float3 SpecularColor;
    float SpecularScalar;
    float3 EmissiveColor;
    float MaterialPad0;
}

struct FDirectionalLight
{
    float3 Direction;
    float Intensity;
    float4 Color;
};

struct FPointLight
{
    float3 Position;
    float Radius;
    
    float4 Color;
    
    float Intensity;
    float AttenuationFalloff;
    float2 pad;
};

struct FSpotLight
{
    float3 Position;
    float Intensity;
    
    float4 Color;
    
    float3 Direction;
    float InnerAngle;
    
    float OuterAngle;
    float3 pad;
};

cbuffer FLightingConstants : register(b1)
{
    uint NumDirectionalLights;
    uint NumPointLights;
    uint NumSpotLights;
    float pad;

    FDirectionalLight DirLights[4];
    FPointLight PointLights[16];
    FSpotLight SpotLights[8];
};

cbuffer FFlagConstants : register(b2)
{
    bool IsLit;
    float3 flagPad0;
}

cbuffer FSubUVConstant : register(b3)
{
    float indexU;
    float indexV;
    float2 Paddd;
}

cbuffer FCameraConstant : register(b4)
{
    matrix ViewMatrix;
    matrix ProjMatrix;
    matrix ViewProjMatrix;
    
    float3 CameraPos;
    float NearPlane;
    float3 CameraForward;
    float FarPlane;
};

cbuffer FComputeConstants : register(b5){
    float4x4 InverseView;
    float4x4 InverseProj;
    float screenWidth;
    float screenHeight;
    int tileCountX;
    int tileCountY;
}

struct PS_INPUT
{
    float4 position : SV_POSITION; // 변환된 화면 좌표
    float3 worldPos : POSITION;
    float4 color : COLOR; // 전달할 색상
    float2 texcoord : TEXCOORD0;
    float3 normal : TEXCOORD1;
    int bHasTex : TEXCOORD2;
    float3x3 TBN: TEXCOORD3;
};

struct PS_OUTPUT
{
    float4 color : SV_Target0;
};

float3 CalculateDirectionalLight(  
    FDirectionalLight Light,  
    float3 Normal,  
    float3 ViewDir,  
    float3 Albedo)  
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
    float3 specularColor = Light.Color.rgb * Specular * SpecularColor;  

    // 최종 광원 영향  
    return (Diffuse + specularColor) * Light.Intensity;  
}

float3 CalculatePointLight(  
    FPointLight Light,  
    float3 WorldPos,  
    float3 Normal,  
    float3 ViewDir,  
    float3 Albedo)  
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
    
#if LIGHTING_MODEL_LAMBERT
    return Diffuse;
#endif
    // 스페큘러  
    float3 HalfVec = normalize(LightDir + ViewDir);  
    float NdotH = max(dot(Normal, HalfVec), 0.0);  
    float Specular = pow(NdotH, SpecularScalar * 64.0) * SpecularScalar;  
    float3 specularColor = Light.Color.rgb * Specular * SpecularColor;

    return (Diffuse + specularColor) * Attenuation;  
}  

float3 CalculateSpotLight(
    FSpotLight Light,
    float3 WorldPos,
    float3 Normal,
    float3 ViewDir,
    float3 Albedo)
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

    // 스페큘러 (Blinn-Phong)
    float3 HalfVec = normalize(SpotDirection + ViewDir);
    float NdotH = max(dot(Normal, HalfVec), 0.0);
    float Specular = pow(NdotH, SpecularScalar * 128.0) * SpecularScalar;
    float3 specularColor = Light.Color.rgb * Specular * SpecularColor;

    return (Diffuse + specularColor) * Light.Intensity * SpotAttenuation * DistanceAttenuation;
}

float2 CalculateUVWithNDCPosition(float4 Position)
{
    float3 NDC = Position.xyz / Position.w;
    NDC.y *= -1;
    return ((NDC + 1) / 2).xy;
}

PS_OUTPUT mainPS(PS_INPUT input)
{
    PS_OUTPUT output;
    float2 uvAdjusted = input.texcoord;
    
    // 기본 색상 추출  
    float4 baseColor = Texture.Sample(linearSampler, uvAdjusted) + float4(DiffuseColor, 1.0);  

#if LIGHTING_MODEL_GOURAUD
    output.color = float4(baseColor.rgb * input.color.rgb, 1.0);
    return output;
#endif
    float4 normalTex = ((NormalTexture.Sample(linearSampler, uvAdjusted)- 0.5) * 2);
    
    uint totalLightCount = NumPointLights; //지금은 PointLight만 하는중

    float tileSizeX = screenWidth / tileCountX;
    float tileSizeY = screenHeight / tileCountY;
    float2 tileSize = float2(tileSizeX, tileSizeY);
    
    float2 uv = CalculateUVWithNDCPosition(input.position);
    uint2 uvToTile = clamp(uint2(uv.xy * tileSize), uint2(0,0), uint2(tileSize - 1));
    int tileIndex = uvToTile.y * tileSizeX + uvToTile.x;
    
    if(!IsLit)
    {
        output.color = float4(baseColor.rgb, 1.0);
        return output;
    }
    
    float3 Normal = input.normal;
    
    if (input.bHasTex)
    {
        Normal = normalize(mul(normalTex.rgb, input.TBN));

        if (length(Normal) < 0.001) // tangent 값이 없을때 ( uv 없을때 )
        {
            Normal = input.normal;
        }        
    }
    
    float3 ViewDir = normalize(CameraPos - input.worldPos);
    
    //float3 TotalLight = MatAmbientColor; // 전역 앰비언트
    // TODO : Lit이면 낮은 값 Unlit이면 float3(1.0f,1.0f,1.0f)면 됩니다. 
    float3 TotalLight = float3(0.01f,0.01f,0.01f); // 전역 앰비언트  
    TotalLight += EmissiveColor; // 자체 발광  

    // 방향광 처리  
    for(uint i=0; i<NumDirectionalLights; ++i)  
        TotalLight += CalculateDirectionalLight(DirLights[i], Normal, ViewDir, baseColor.rgb);  

    // 점광 처리  
    for(uint j=0; j<NumPointLights; ++j)
    {
        uint listIndex = tileIndex * tileCountX + j; //여기 포문 들어가서 해야할듯
        uint lightIndex = TileLightIndices[listIndex];
        if (lightIndex == 0xFFFFFFFF)
        {
            break;
        }
        
        TotalLight += CalculatePointLight(PointLights[lightIndex], input.worldPos, Normal, ViewDir, baseColor.rgb);
        // output.color = float4(1,1,1,1);
        // return output;
    }
    
    for (uint k = 0; k < NumSpotLights; ++k)
        TotalLight += CalculateSpotLight(SpotLights[k], input.worldPos, Normal, ViewDir, baseColor.rgb);
    
    // 최종 색상 
    output.color = float4(TotalLight*baseColor.rgb, baseColor.a * TransparencyScalar);
    return output;  
}