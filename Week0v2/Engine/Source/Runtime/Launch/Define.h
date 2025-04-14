#pragma once
#include <cmath>
#include <algorithm>
#include "Core/Container/String.h"
#include "Core/Container/Array.h"
#include "UObject/NameTypes.h"

// 수학 관련
#include "Math/Vector.h"
#include "Math/Vector4.h"

#define UE_LOG Console::GetInstance().AddLog

#define _TCHAR_DEFINED
#include <d3d11.h>

#include "UserInterface/Console.h"
#include "Serialization/Archive.h"


struct FVertexSimple
{
    float x, y, z, w;    // Position
    float r, g, b, a; // Color
    float nx, ny, nz;
    float u=0, v=0;
    uint32 MaterialIndex;
};

// Material Subset
struct FMaterialSubset
{
    uint32 IndexStart; // Index Buffer Start pos
    uint32 IndexCount; // Index Count
    uint32 MaterialIndex; // Material Index
    FString MaterialName; // Material Name
};

struct FStaticMaterial
{
    class UMaterial* Material;
    FName MaterialSlotName;
    //FMeshUVChannelInfo UVChannelData;
};

// OBJ File Raw Data
struct FObjInfo
{
    FWString ObjectName; // OBJ File Name
    FWString PathName; // OBJ File Paths
    FString DisplayName; // Display Name
    FString MatName; // OBJ MTL File Name
    
    // Group
    uint32 NumOfGroup = 0; // token 'g' or 'o'
    TArray<FString> GroupName;
    
    // Vertex, UV, Normal List
    TArray<FVector> Vertices;
    TArray<FVector> Normals;
    TArray<FVector2D> UVs;
    
    // Faces
    TArray<int32> Faces;

    // Index
    TArray<uint32> VertexIndices;
    TArray<uint32> NormalIndices;
    TArray<uint32> TextureIndices;
    
    // Material
    TArray<FMaterialSubset> MaterialSubsets;
};

struct FObjMaterialInfo
{
    FString MTLName;  // newmtl : Material Name.

    bool bHasTexture = false;  // Has Texture?
    bool bTransparent = false; // Has alpha channel?

    FVector Diffuse;  // Kd : Diffuse (Vector4)
    FVector Specular;  // Ks : Specular (Vector) 
    FVector Ambient;   // Ka : Ambient (Vector)
    FVector Emissive;  // Ke : Emissive (Vector)

    float SpecularScalar; // Ns : Specular Power (Float)
    float DensityScalar;  // Ni : Optical Density (Float)
    float TransparencyScalar; // d or Tr  : Transparency of surface (Float)

    uint32 IlluminanceModel; // illum: illumination Model between 0 and 10. (UINT)

    /* Texture */
    FString DiffuseTextureName;  // map_Kd : Diffuse texture
    FWString DiffuseTexturePath;
    
    FString AmbientTextureName;  // map_Ka : Ambient texture
    FWString AmbientTexturePath;
    
    FString SpecularTextureName; // map_Ks : Specular texture
    FWString SpecularTexturePath;
    
    FString BumpTextureName;     // map_Bump : Bump texture
    FWString BumpTexturePath;
    
    FString AlphaTextureName;    // map_d : Alpha texture
    FWString AlphaTexturePath;
};

// Cooked Data
namespace OBJ
{
    struct FStaticMeshRenderData
    {
        FWString ObjectName;
        FWString PathName;
        FString DisplayName;
        
        TArray<FVertexSimple> Vertices;
        TArray<UINT> Indices;

        FString VBName;
        FString IBName;
        // ID3D11Buffer* VertexBuffer;
        // ID3D11Buffer* IndexBuffer;
        
        TArray<FObjMaterialInfo> Materials;
        TArray<FMaterialSubset> MaterialSubsets;

        FVector BoundingBoxMin;
        FVector BoundingBoxMax;
    };
}

enum class EShaderStage
{
    VS,		// Vertex Shader
    HS,		// Hull Shader
    DS,		// Domain Shader
    GS,		// Geometry Shader
    PS,		// Pixel Shader
    CS,		// Compute Shader
    All,
    End,
};

// ShaderType과 Constant 이름을 결합한 키 구조체
struct FShaderConstantKey
{
    EShaderStage ShaderType;  // 예: Vertex, Pixel 등
    FName ConstantName;    // 상수 버퍼 내 상수 이름

    // 동등 비교 연산자: 두 키가 동일하면 true
    bool operator==(const FShaderConstantKey& Other) const
    {
        return ShaderType == Other.ShaderType && ConstantName == Other.ConstantName;
    }
};

// std::hash 특수화를 통해 FShaderConstantKey를 해시 기반 컨테이너에서 사용할 수 있게 함
namespace std
{
    template<>
    struct hash<FShaderConstantKey>
    {
        std::size_t operator()(const FShaderConstantKey& Key) const noexcept
        {
            // EShaderType은 enum class이므로 int로 캐스팅하여 해시를 계산
            std::size_t h1 = std::hash<uint32>()(static_cast<int>(Key.ShaderType));
            std::size_t h2 = std::hash<FName>()(Key.ConstantName);
            // 간단한 해시 결합: XOR과 쉬프트 사용 (더 복잡한 해시 결합도 가능)
            return h1 ^ (h2 << 1);
        }
    };
}

enum class ESamplerType
{
    Point,
    Linear,
    Anisotropic,
    PostProcess,
    End,
};

enum class ERenderingMode
{
    Opaque,
    CutOut,
    Transparent,
    PostProcess,
    End,
};

enum class ETextureType
{
    Albedo,
    Normal,
    Specular,
    Smoothness,
    Metallic,
    Sprite,
    End,
};

enum class ERasterizerState
{
    SolidBack,
    SolidFront,
    SolidNone,
    WireFrame,
    End,
};

enum class EBlendState
{
    AlphaBlend,
    OneOne,
    End,
};

enum class EDepthStencilState
{
    DepthNone,
    LessEqual,
    End,
};

struct FVertexTexture
{
	float x, y, z;    // Position
	float u, v; // Texture
};

struct FSimpleVertex
{
	float dummy; // 내용은 사용되지 않음
    float padding[11];
};

struct FOBB
{
    FVector corners[8];
};

struct FRect
{
    FRect() : leftTopX(0), leftTopY(0), width(0), height(0) {}
    FRect(float x, float y, float w, float h) : leftTopX(x), leftTopY(y), width(w), height(h) {}
    float leftTopX, leftTopY, width, height;
};

struct FPoint
{
    FPoint() : x(0), y(0) {}
    FPoint(float _x, float _y) : x(_x), y(_y) {}
    FPoint(long _x, long _y) : x(_x), y(_y) {}
    FPoint(int _x, int _y) : x(_x), y(_y) {}

    float x, y;
};

struct FBoundingBox
{
    FBoundingBox(){}
    FBoundingBox(FVector _min, FVector _max) : min(_min), max(_max) {}
	FVector min; // Minimum extents
	float pad;
	FVector max; // Maximum extents
	float pad1;
    bool Intersect(const FVector& rayOrigin, const FVector& rayDir, float& outDistance)
    {
        float tmin = -FLT_MAX;
        float tmax = FLT_MAX;
        const float epsilon = 1e-6f;

        // X축 처리
        if (fabs(rayDir.x) < epsilon)
        {
            // 레이가 X축 방향으로 거의 평행한 경우,
            // 원점의 x가 박스 [min.x, max.x] 범위 밖이면 교차 없음
            if (rayOrigin.x < min.x || rayOrigin.x > max.x)
                return false;
        }
        else
        {
            float t1 = (min.x - rayOrigin.x) / rayDir.x;
            float t2 = (max.x - rayOrigin.x) / rayDir.x;
            if (t1 > t2)  std::swap(t1, t2);

            // tmin은 "현재까지의 교차 구간 중 가장 큰 min"
            tmin = (t1 > tmin) ? t1 : tmin;
            // tmax는 "현재까지의 교차 구간 중 가장 작은 max"
            tmax = (t2 < tmax) ? t2 : tmax;
            if (tmin > tmax)
                return false;
        }

        // Y축 처리
        if (fabs(rayDir.y) < epsilon)
        {
            if (rayOrigin.y < min.y || rayOrigin.y > max.y)
                return false;
        }
        else
        {
            float t1 = (min.y - rayOrigin.y) / rayDir.y;
            float t2 = (max.y - rayOrigin.y) / rayDir.y;
            if (t1 > t2)  std::swap(t1, t2);

            tmin = (t1 > tmin) ? t1 : tmin;
            tmax = (t2 < tmax) ? t2 : tmax;
            if (tmin > tmax)
                return false;
        }

        // Z축 처리
        if (fabs(rayDir.z) < epsilon)
        {
            if (rayOrigin.z < min.z || rayOrigin.z > max.z)
                return false;
        }
        else
        {
            float t1 = (min.z - rayOrigin.z) / rayDir.z;
            float t2 = (max.z - rayOrigin.z) / rayDir.z;
            if (t1 > t2)  std::swap(t1, t2);

            tmin = (t1 > tmin) ? t1 : tmin;
            tmax = (t2 < tmax) ? t2 : tmax;
            if (tmin > tmax)
                return false;
        }

        // 여기까지 왔으면 교차 구간 [tmin, tmax]가 유효하다.
        // tmax < 0 이면, 레이가 박스 뒤쪽에서 교차하므로 화면상 보기엔 교차 안 한다고 볼 수 있음
        if (tmax < 0.0f)
            return false;

        // outDistance = tmin이 0보다 크면 그게 레이가 처음으로 박스를 만나는 지점
        // 만약 tmin < 0 이면, 레이의 시작점이 박스 내부에 있다는 의미이므로, 거리를 0으로 처리해도 됨.
        outDistance = (tmin >= 0.0f) ? tmin : 0.0f;

        return true;
    }
    void Serialize(FArchive& Ar) const
    {
        Ar << min << max;
    }
    void Deserialize(FArchive& Ar)
    {
        Ar >> min >> max;
    }
};

struct FCone
{
    FVector ConeApex; // 원뿔의 꼭짓점
    float ConeRadius; // 원뿔 밑면 반지름

    FVector ConeBaseCenter; // 원뿔 밑면 중심
    float ConeHeight; // 원뿔 높이 (Apex와 BaseCenter 간 차이)
    FVector4 Color;

    int ConeSegmentCount; // 원뿔 밑면 분할 수
    float pad[3];

};

// !NOTE : 최대 광원 수 제한
#define MAX_DIRECTIONAL_LIGHTS 4
#define MAX_POINT_LIGHTS 16

struct FDirectionalLight
{
    FVector Direction;
    float Intensity;

    FVector Color;
    float Padding0 = 0.f;
};

struct FPointLight
{
    FVector Position;
    float Radius;

    FVector Color;
    float Intensity;

    float AttenuationFalloff;
    FVector Pad;
};
