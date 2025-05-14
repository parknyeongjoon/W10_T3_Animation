#define NUM_POINT_LIGHT 4
#define NUM_SPOT_LIGHT 4

static const int CASCADE_COUNT = 4;

cbuffer FMatrixConstants : register(b0)
{
    row_major float4x4 Model;
    row_major float4x4 ViewProj;
    row_major float4x4 MInverseTranspose;
    bool isSelected;
    float3 pad0;
};

struct VS_INPUT
{
    float4 position : POSITION; // 버텍스 위치
    float4 color : COLOR; // 버텍스 색상
    float3 normal : NORMAL; // 버텍스 노멀
    float3 tangent : TANGENT;
    float2 texcoord : TEXCOORD;
};

struct PS_INPUT
{
    float4 position : SV_POSITION; // 변환된 화면 좌표
    float3 worldPos : POSITION;
    float4 color : COLOR; // 전달할 색상
    float2 texcoord : TEXCOORD0;
    float3 normal : TEXCOORD1;
    float3x3 TBN : TEXCOORD3;
};

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

cbuffer FLightingConstants : register(b1)
{
    uint NumPointLights;
    uint NumSpotLights;
    float2 pad2;

    FDirectionalLight DirLight;
    FPointLight PointLights[8];
    FSpotLight SpotLights[8];
};

cbuffer FFlagConstants : register(b2)
{
    uint IsLit;
    uint IsNormal;
    uint IsVSM;
    uint IsGPUSkinning;
}

cbuffer FCameraConstant : register(b3)
{
    matrix ViewMatrix;
    matrix ProjMatrix;
    matrix ViewProjMatrix;
    
    float3 CameraPos;
    float NearPlane;
    float3 CameraForward;
    float FarPlane;
};

cbuffer FMaterialConstants : register(b4)
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
    if (Distance > Light.Radius)
        return float3(0, 0, 0);

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

PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;
    
    float4 worldPos = mul(input.position, Model);
    output.position = mul(worldPos, ViewProj);
    output.worldPos = float3(worldPos.x, worldPos.y, worldPos.z);
    output.color = input.color;
    output.texcoord = input.texcoord;
    
    float3 normal = mul(float4(input.normal, 0), MInverseTranspose).xyz;
    
#if LIGHTING_MODEL_GOURAUD
    float3 viewDir = normalize(CameraPos - worldPos.xyz);
    
    // float3 totalLight = MatAmbientColor + EmissiveColor;
    //TODO : Lit 이면 기본 Ambient를 낮은 값으로 하고 Unlit이면 float(1.0,1.0,1.0)으로 하면 됩니다.
    //기본적으로 머터리얼을 읽어올 때는 1.0,1.0,1.0으로 읽어오는 것 같아요 
    float3 totalLight = float3(0.01f,0.01f,0.01f) + EmissiveColor;
    
    if(!IsLit)
    {
        output.color = float4(input.color.rgb, 1.0);
        return output;
    }
    // 정점 색상 계산 (디퓨즈 + 스페큘러)
    totalLight += CalculateDirectionalLight(DirLight, normal, viewDir, input.color.rgb);

    // 점광 처리  
    for(uint j=0; j<NumPointLights; ++j)  
        totalLight += CalculatePointLight(PointLights[j], worldPos.xyz, normal, viewDir, input.color.rgb);  

    //SpotLight 처리
    for(uint k=0; k<NumSpotLights; ++k)
        totalLight += CalculateSpotLight(SpotLights[k], worldPos.xyz, normal, viewDir, input.color.rgb);

    // 정점 셰이더에서 계산된 색상을 픽셀 셰이더로 전달
    output.color = float4(totalLight * input.color.rgb, input.color.a * TransparencyScalar);  
        
    return output;
#endif
    float3 tangent = normalize(mul(input.tangent, Model));

    // 탄젠트-노멀 직교화 (Gram-Schmidt 과정) 해야 안전함
    tangent = normalize(tangent - normal * dot(tangent, normal));

    // 바이탄젠트 계산 (안전한 교차곱)
    float3 biTangent = cross(normal, tangent);

    // 최종 TBN 행렬 구성 (T, B, N는 각각 한 열 또는 행이 될 수 있음, 아래 예제는 행 벡터로 구성)
    // row_major float4x4 TBN = float4x4(T, B, N, float4(0,0,0,1));
    float3x3 TBN = float3x3(tangent, biTangent, normal);

    output.TBN = TBN;
    output.normal = normal;

    return output;
}
