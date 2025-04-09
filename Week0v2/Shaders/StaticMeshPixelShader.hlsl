Texture2D Textures : register(t0);
SamplerState Sampler : register(s0);

cbuffer MatrixConstants : register(b0)
{
    row_major matrix Model;
    row_major matrix ViewProj;
    row_major matrix MInverseTranspose;
    float4 UUID;
    bool isSelected;
    float3 MatrixPad0;
};

struct FMaterial
{
    float3 DiffuseColor;
    float TransparencyScalar;
    float3 AmbientColor;
    float DensityScalar;
    float3 SpecularColor;
    float SpecularScalar;
    float3 EmissiveColor;
    float MaterialPad0;
};

cbuffer MaterialConstants : register(b1)
{
    FMaterial Material;
}

struct FDirectionalLight
{
    float3 Direction;
    float Intensity;
    float3 Color;
    float pad1;
};

struct FPointLight
{
    float3 Position;
    float Radius;
    
    float3 Color;
    float Intensity;
    
    float AttenuationFalloff;
    float3 pad;
};

cbuffer LightingConstants : register(b2)
{
    float3 AmbientColor;
    float AmbientIntensity;

    uint NumDirectionalLights;
    uint NumPointLights;
    float2 pad;

    FDirectionalLight DirLights[4];
    FPointLight PointLights[16];
};

cbuffer FlagConstants : register(b3)
{
    bool IsLit;
    float3 flagPad0;
}

cbuffer SubMeshConstants : register(b4)
{
    bool IsSelectedSubMesh;
    float3 SubMeshPad0;
}

cbuffer TextureConstants : register(b5)
{
    float2 UVOffset;
    float2 TexturePad0;
}

cbuffer CameraConstant : register(b6)
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
    float3 normal : NORMAL; // 정규화된 노멀 벡터
    bool normalFlag : TEXCOORD0; // 노멀 유효성 플래그 (1.0: 유효, 0.0: 무효)
    float2 texcoord : TEXCOORD1;
    int materialIndex : MATERIAL_INDEX;
};

struct PS_OUTPUT
{
    float4 color : SV_Target0;
    float4 UUID : SV_Target1;
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
float3 CalculateDirectionalLight(FDirectionalLight Light, float3 Normal, float3 ViewDir, float3 DiffuseColor, float3 SpecularColor, float3 SpecularPower)
{
    float3 LightDir = normalize(-Light.Direction);
    float Diff = max(dot(Normal, LightDir), 0.0f);
    
    float3 Diffuse = Light.Color * Diff * DiffuseColor * Light.Intensity;
    
    float3 ReflectDir = reflect(-LightDir, Normal);
    float Spec = pow(max(dot(ViewDir, ReflectDir), 0.0f), SpecularPower);
    float3 Specular = Light.Color * SpecularColor * Spec * Light.Intensity;
    
    return Diffuse + Specular;
}

float3 CalculatePointLight(FPointLight Light, float3 WorldPos, float3 Normal, float3 ViewDir, float3 DiffuseColor, float3 SpecularColor, float3 SpecularPower)
{
    float3 LightDir = normalize(Light.Position - WorldPos);
    float Distance = length(Light.Position - WorldPos);
    
    if (Distance > Light.Radius)
        return float3(0, 0, 0);
    
    float NormalizedDistance = saturate(Distance / Light.Radius);
    
    float RadiusAttenuation = 1.0 - NormalizedDistance * NormalizedDistance; // 부드러운 경계 추가
    float DistanceAttenuation = 1.0f / (1.0f + Light.AttenuationFalloff * Distance * Distance);
    
    float Attenuation = RadiusAttenuation * DistanceAttenuation * Light.Intensity;

    float Diff = max(dot(Normal, LightDir), 0.0f);
    float3 Diffuse = Light.Color * Diff * DiffuseColor * Attenuation; // float3으로 수정
    
    float3 ReflectDir = reflect(-LightDir, Normal);
    float Spec = pow(max(dot(ViewDir, ReflectDir), 0.0f), SpecularPower);
    float3 Specular = Light.Color * SpecularColor * Spec * Attenuation;
    
    return Diffuse + Specular;
}

PS_OUTPUT mainPS(PS_INPUT input)
{
    PS_OUTPUT output;
    
    output.UUID = UUID;
    
    float4 baseColor = Textures.Sample(Sampler, input.texcoord + UVOffset) + float4(Material.DiffuseColor, Material.TransparencyScalar);
    
    if(!IsLit)
    {
        output.color = float4(baseColor.rgb, 1.0);
        return output;
    }
    
    float3 Normal = normalize(input.normal);
    float3 ViewDir = normalize(CameraPos - input.worldPos);
    
    float3 result = AmbientColor * AmbientIntensity * baseColor.rgb;
    
    for (uint i = 0; i < NumDirectionalLights; i++)
    {
        result += CalculateDirectionalLight(DirLights[i], Normal, ViewDir, baseColor.rgb, Material.SpecularColor, Material.SpecularScalar);
    }
    
    for (uint j = 0; j < NumPointLights; j++)
    {
        result += CalculatePointLight(PointLights[j], input.worldPos, Normal, ViewDir, baseColor.rgb, Material.SpecularColor, Material.SpecularScalar);
    }
    
    output.color = float4(result, baseColor.a);
    
    return output;
}