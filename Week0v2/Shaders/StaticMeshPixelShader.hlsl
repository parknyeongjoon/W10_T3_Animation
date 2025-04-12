#include "ShaderHeaders/GSamplers.hlsli"
Texture2D Textures : register(t0);

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

cbuffer FLightingConstants : register(b1)
{
    uint NumDirectionalLights;
    uint NumPointLights;
    float2 pad;

    FDirectionalLight DirLights[4];
    FPointLight PointLights[16];
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

struct PS_INPUT
{
    float4 position : SV_POSITION; // 변환된 화면 좌표
    float3 worldPos : POSITION;
    float4 color : COLOR; // 전달할 색상
    float2 texcoord : TEXCOORD0;
    float3 tangent: TEXCOORD1;
    float3 biTangent: TEXCOORD2;
    float3 normal: TEXCOORD3;
};

struct PS_OUTPUT
{
    float4 color : SV_Target0;
};

float noise(float3 p)
{
    return frac(sin(dot(p, float3(12.9898, 78.233, 37.719))) * 43758.5453);
}

float4 PaperTexture(float3 originalColor)
{
    // 입력 색상을 0~1 범위로 제한
    float3 color = saturate(originalColor);
    
    float3 paperColor = float3(0.95, 0.95, 0.95);
    float blendFactor = 0.5;
    float3 mixedColor = lerp(color, paperColor, blendFactor);
    
    // 정적 grain 효과
    float grain = noise(color * 10.0) * 0.1;
    
    // 거친 질감 효과: 두 단계의 노이즈 레이어를 결합
    float rough1 = (noise(color * 20.0) - 0.5) * 0.15; // 첫 번째 레이어: 큰 규모의 노이즈
    float rough2 = (noise(color * 40.0) - 0.5) * 0.01; // 두 번째 레이어: 세부 질감 추가
    float rough = rough1 + rough2;
    
    // vignette 효과: 중앙에서 멀어질수록 어두워지는 효과
    float vignetting = smoothstep(0.4, 1.0, length(color.xy - 0.5) * 2.0);
    
    // 최종 색상 계산
    float3 finalColor = mixedColor + grain + rough - vignetting * 0.1;
    return float4(saturate(finalColor), 1.0);
}

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

    // 스페큘러  
    float3 HalfVec = normalize(LightDir + ViewDir);  
    float NdotH = max(dot(Normal, HalfVec), 0.0);  
    float Specular = pow(NdotH, SpecularScalar * 64.0) * SpecularScalar;  
    float3 specularColor = Light.Color.rgb * Specular * SpecularColor;  

    return (Diffuse + specularColor) * Attenuation;  
}  

PS_OUTPUT mainPS(PS_INPUT input)
{
    PS_OUTPUT output;
    
    float2 uvAdjusted = input.texcoord + float2(indexU, indexV);
    
    // 기본 색상 추출  
    float4 baseColor = Textures.Sample(linearSampler, uvAdjusted) + float4(DiffuseColor, 1.0);  

    input.normal = (input.normal - 0.5) * 2;
    
    if(!IsLit)
    {
        output.color = float4(baseColor.rgb, 1.0);
        return output;
    }

    float3x3 TBN = float3x3( 
        normalize(input.tangent), //이미 normalize 된 상태여서 안해도 될듯?
        normalize(input.biTangent),
        normalize(input.normal)
    );
    
    float3 Normal = normalize(mul(input.normal, TBN));
    float3 ViewDir = normalize(CameraPos - input.worldPos);
    
    float3 TotalLight = MatAmbientColor; // 전역 앰비언트  
    TotalLight += EmissiveColor; // 자체 발광  

    // 방향광 처리  
    for(uint i=0; i<NumDirectionalLights; ++i)  
        TotalLight += CalculateDirectionalLight(DirLights[i], Normal, ViewDir, baseColor.rgb);  

    // 점광 처리  
    for(uint j=0; j<NumPointLights; ++j)  
        TotalLight += CalculatePointLight(PointLights[j], input.worldPos, Normal, ViewDir, baseColor.rgb);  

    // 최종 색상  
    output.color = float4(TotalLight * baseColor.rgb, baseColor.a * TransparencyScalar);  
    return output;  
}