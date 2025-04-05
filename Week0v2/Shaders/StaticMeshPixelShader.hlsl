Texture2D Textures : register(t0);
SamplerState Sampler : register(s0);

cbuffer MatrixConstants : register(b0)
{
    row_major float4x4 MVP;
    row_major float4x4 MInverseTranspose;
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
    
    float Attenuation;
    float pad[3];
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
float3 ApplyLighting(float3 worldPos, float3 normal, float3 albedo, float3 specularColor, float specularScalar)
{
    float3 result = AmbientColor * AmbientIntensity;
    
    float3 V = normalize(CameraPos - worldPos);
    // 디렉셔널 라이트 계산
    for (uint i = 0; i < NumDirectionalLights; ++i)
    {
        float3 L = normalize(-DirLights[i].Direction);
        float3 H = normalize(L + V);
        
        float NDotL = max(dot(normal, L), 0.0);
        float NDotH = max(dot(normal, H), 0.0);
        
        float diff = NDotL;
        float spec = pow(NDotH, specularScalar * 32) * specularScalar;
        
        float3 lightColor = DirLights[i].Color * DirLights[i].Intensity;
        result += (albedo * diff + specularColor * spec) * lightColor;
    }
    
    // 포인트 라이트 계산
    for (uint i = 0; i < NumPointLights; ++i)
    {
        float3 L = PointLights[i].Position - worldPos;
        float distance = length(L);
        L = normalize(L);
        float3 H = normalize(L + V);

        float NDotL = max(dot(normal, L), 0.0);
        float NDotH = max(dot(normal, H), 0.0);
        float diff = NDotL;
        float spec = pow(saturate(dot(normal, H)), specularScalar * 32) * specularScalar;
        
        float attenuation = PointLights[i].Attenuation;

        float3 lightColor = PointLights[i].Color * PointLights[i].Intensity;
        result += (albedo * diff + specularColor * spec) * lightColor * attenuation;
    }
    
    return result;
}

PS_OUTPUT mainPS(PS_INPUT input)
{
    PS_OUTPUT output;
    
    output.UUID = UUID;
    
    float3 texColor = Textures.Sample(Sampler, input.texcoord + UVOffset);
    float3 color = (texColor.g == 0) ? saturate(Material.DiffuseColor) : texColor + Material.DiffuseColor;
    
    if (isSelected)
    {
        color += float3(0.2f, 0.2f, 0.0f); // 노란색 틴트로 하이라이트
        if (IsSelectedSubMesh)
            color = float3(1, 1, 1);
    }
    
    float3 finalColor = color;
    // 발광 색상 추가

    if(IsLit == 1 && input.normalFlag > 0.5f)
    {
        float3 normal = normalize(input.normal);
        float3 worldPos = input.position;
        
        finalColor = ApplyLighting(worldPos, normal, color, Material.SpecularColor, Material.SpecularScalar);
        finalColor += Material.EmissiveColor;
    }

    output.color = float4(finalColor, Material.TransparencyScalar);
    return output;
}