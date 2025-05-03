#pragma once
#include "Container/String.h"
#include "Math/Vector.h"

struct FMaterialInfo
{
    FString MaterialName;  // Material Name.

    bool bHasTexture = false;  // Has Texture?
    bool bTransparent = false; // Has alpha channel?

    // Diffuse (Kd) : 일반적으로 흰색, 완전 불투명한 색상
    FVector Diffuse = FVector(0.0f, 0.0f, 0.0f);
    
    // Specular (Ks) : 반사광 기본값, 흰색으로 표기하는 경우가 많음
    FVector Specular = FVector(0.0f, 0.0f, 0.0f);
    
    // Ambient (Ka) : 주변광 기본값, 너무 강하지 않은 낮은 값으로
    FVector Ambient = FVector(0.1f, 0.1f, 0.1f);
    
    // Emissive (Ke) : 자체 발광 없음
    FVector Emissive = FVector(0.0f, 0.0f, 0.0f);
    
    // SpecularScalar (Ns) : 스페큘러 파워 (보통 1.0 이상, 필요에 따라 조정)
    float SpecularScalar = 1.0f;
    
    // DensityScalar (Ni) : 광학적 밀도(굴절률 등), 기본적으로 1.0
    float DensityScalar = 1.0f;
    
    // TransparencyScalar : 투명도, 1.0이면 불투명, 0.0이면 완전 투명
    float TransparencyScalar = 1.0f;

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

    FString NormalTextureName;  // map_Ns : Normal Texture
    FWString NormalTexturePath;

    float NormalScale = 1.0f;
};
