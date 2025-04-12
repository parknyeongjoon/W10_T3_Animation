// Auto-generated constant buffer structures with padding
#pragma once

#include "HAL/PlatformType.h"
#include "Math/Matrix.h"
#include "Math/Vector.h"
#include "Math/Vector4.h"

// NOTE: Generated code - do not modify manually.

struct alignas(16) FCameraConstant
{
    FMatrix ViewMatrix; // offset: 0, size: 64
    FMatrix ProjMatrix; // offset: 64, size: 64
    FMatrix InvViewMatrix; // offset: 128, size: 64
    FMatrix InvProjMatrix; // offset: 192, size: 64
    FVector CameraPos; // offset: 256, size: 12
    float NearPlane; // offset: 268, size: 4
    FVector CameraForward; // offset: 272, size: 12
    float FarPlane; // offset: 284, size: 4
};

struct alignas(16) FViewportInfo
{
    FVector2D ViewportSize; // offset: 0, size: 8
    FVector2D ViewportOffset; // offset: 8, size: 8
};

struct alignas(16) FFogParams
{
    float FogDensity; // offset: 0, size: 4
    float HeightFogStart; // offset: 4, size: 4
    float HeightFogEnd; // offset: 8, size: 4
    float MaxOpacity; // offset: 12, size: 4
    float DistanceFogNear; // offset: 16, size: 4
    float DistanceFotFar; // offset: 20, size: 4
    float pad1; // offset: 24, size: 4
    float pad2; // offset: 28, size: 4
    FVector4 InscatteringColor; // offset: 32, size: 16
    FVector4 DirectionalInscatteringColor; // offset: 48, size: 16
    FVector DirectionalLightDirection; // offset: 64, size: 12
    float DirectionalInscatteringExponent; // offset: 76, size: 4
    float DirectionalInscatteringStartDistance; // offset: 80, size: 4
    float pad3; // offset: 84, size: 4
    float pad4; // offset: 88, size: 4
    float pad5; // offset: 92, size: 4
    int IsExponential; // offset: 96, size: 4
    uint8 pad0[12]; // Padding to end of buffer
};

struct alignas(16) FMatrixBuffer
{
    FMatrix Model; // offset: 0, size: 64
    FMatrix ViewProj; // offset: 64, size: 64
};

struct alignas(16) FGridParametersData
{
    float GridSpacing; // offset: 0, size: 4
    int GridCount; // offset: 4, size: 4
    uint8 pad0[8]; // Padding from offset 8 to 16
    FVector GridOrigin; // offset: 16, size: 12
    float Padding; // offset: 28, size: 4
};

struct alignas(16) FPrimitiveCounts
{
    int BoundingBoxCount; // offset: 0, size: 4
    int pad; // offset: 4, size: 4
    int ConeCount; // offset: 8, size: 4
    int pad1; // offset: 12, size: 4
};

struct alignas(16) FMaterialConstants
{
    FVector DiffuseColor; // offset: 0, size: 12
    float TransparencyScalar; // offset: 12, size: 4
    FVector MatAmbientColor; // offset: 16, size: 12
    float DensityScalar; // offset: 28, size: 4
    FVector SpecularColor; // offset: 32, size: 12
    float SpecularScalar; // offset: 44, size: 4
    FVector EmissiveColor; // offset: 48, size: 12
    float MaterialPad0; // offset: 60, size: 4
};

struct alignas(16) FLightingConstants
{
    FVector AmbientColor; // offset: 0, size: 12
    float AmbientIntensity; // offset: 12, size: 4
    uint32 NumDirectionalLights; // offset: 16, size: 4
    uint32 NumPointLights; // offset: 20, size: 4
    FVector2D pad; // offset: 24, size: 8
    FDirectionalLight DirLights[4]; // offset: 32, size: 128
    FPointLight PointLights[16]; // offset: 160, size: 768
};

struct alignas(16) FFlagConstants
{
    bool IsLit; // offset: 0, size: 4
    FVector flagPad0; // offset: 4, size: 12
};

struct alignas(16) FSubUVConstant
{
    float indexU; // offset: 0, size: 4
    float indexV; // offset: 4, size: 4
    uint8 pad0[8]; // Padding to end of buffer
};

struct alignas(16) FMatrixConstants
{
    FMatrix Model; // offset: 0, size: 64
    FMatrix ViewProj; // offset: 64, size: 64
    FMatrix MInverseTranspose; // offset: 128, size: 64
    bool isSelected; // offset: 192, size: 4
    uint8 pad0[12]; // Padding to end of buffer
};

struct alignas(16) FConstants
{
    FMatrix Model; // offset: 0, size: 64
    FMatrix ViewProj; // offset: 64, size: 64
    float Flag; // offset: 128, size: 4
    uint8 pad0[12]; // Padding to end of buffer
};

enum class EShaderConstantBuffer
{
    FCameraConstant = 0,
    FConstants = 1,
    FFlagConstants = 2,
    FFogParams = 3,
    FGridParametersData = 4,
    FLightingConstants = 5,
    FMaterialConstants = 6,
    FMatrixBuffer = 7,
    FMatrixConstants = 8,
    FPrimitiveCounts = 9,
    FSubUVConstant = 10,
    FViewportInfo = 11,
    EShaderConstantBuffer_MAX
};

inline const TCHAR* EShaderConstantBufferToString(EShaderConstantBuffer e)
{
    switch(e)
    {
    case EShaderConstantBuffer::FCameraConstant: return TEXT("FCameraConstant");
    case EShaderConstantBuffer::FConstants: return TEXT("FConstants");
    case EShaderConstantBuffer::FFlagConstants: return TEXT("FFlagConstants");
    case EShaderConstantBuffer::FFogParams: return TEXT("FFogParams");
    case EShaderConstantBuffer::FGridParametersData: return TEXT("FGridParametersData");
    case EShaderConstantBuffer::FLightingConstants: return TEXT("FLightingConstants");
    case EShaderConstantBuffer::FMaterialConstants: return TEXT("FMaterialConstants");
    case EShaderConstantBuffer::FMatrixBuffer: return TEXT("FMatrixBuffer");
    case EShaderConstantBuffer::FMatrixConstants: return TEXT("FMatrixConstants");
    case EShaderConstantBuffer::FPrimitiveCounts: return TEXT("FPrimitiveCounts");
    case EShaderConstantBuffer::FSubUVConstant: return TEXT("FSubUVConstant");
    case EShaderConstantBuffer::FViewportInfo: return TEXT("FViewportInfo");
    default: return TEXT("unknown");
    }
}

inline EShaderConstantBuffer EShaderConstantBufferFromString(const TCHAR* str)
{
#if USE_WIDECHAR
    if(std::wcscmp(str, TEXT("FCameraConstant")) == 0) return EShaderConstantBuffer::FCameraConstant;
    if(std::wcscmp(str, TEXT("FConstants")) == 0) return EShaderConstantBuffer::FConstants;
    if(std::wcscmp(str, TEXT("FFlagConstants")) == 0) return EShaderConstantBuffer::FFlagConstants;
    if(std::wcscmp(str, TEXT("FFogParams")) == 0) return EShaderConstantBuffer::FFogParams;
    if(std::wcscmp(str, TEXT("FGridParametersData")) == 0) return EShaderConstantBuffer::FGridParametersData;
    if(std::wcscmp(str, TEXT("FLightingConstants")) == 0) return EShaderConstantBuffer::FLightingConstants;
    if(std::wcscmp(str, TEXT("FMaterialConstants")) == 0) return EShaderConstantBuffer::FMaterialConstants;
    if(std::wcscmp(str, TEXT("FMatrixBuffer")) == 0) return EShaderConstantBuffer::FMatrixBuffer;
    if(std::wcscmp(str, TEXT("FMatrixConstants")) == 0) return EShaderConstantBuffer::FMatrixConstants;
    if(std::wcscmp(str, TEXT("FPrimitiveCounts")) == 0) return EShaderConstantBuffer::FPrimitiveCounts;
    if(std::wcscmp(str, TEXT("FSubUVConstant")) == 0) return EShaderConstantBuffer::FSubUVConstant;
    if(std::wcscmp(str, TEXT("FViewportInfo")) == 0) return EShaderConstantBuffer::FViewportInfo;
#else
    if(std::strcmp(str, "FCameraConstant") == 0) return EShaderConstantBuffer::FCameraConstant;
    if(std::strcmp(str, "FConstants") == 0) return EShaderConstantBuffer::FConstants;
    if(std::strcmp(str, "FFlagConstants") == 0) return EShaderConstantBuffer::FFlagConstants;
    if(std::strcmp(str, "FFogParams") == 0) return EShaderConstantBuffer::FFogParams;
    if(std::strcmp(str, "FGridParametersData") == 0) return EShaderConstantBuffer::FGridParametersData;
    if(std::strcmp(str, "FLightingConstants") == 0) return EShaderConstantBuffer::FLightingConstants;
    if(std::strcmp(str, "FMaterialConstants") == 0) return EShaderConstantBuffer::FMaterialConstants;
    if(std::strcmp(str, "FMatrixBuffer") == 0) return EShaderConstantBuffer::FMatrixBuffer;
    if(std::strcmp(str, "FMatrixConstants") == 0) return EShaderConstantBuffer::FMatrixConstants;
    if(std::strcmp(str, "FPrimitiveCounts") == 0) return EShaderConstantBuffer::FPrimitiveCounts;
    if(std::strcmp(str, "FSubUVConstant") == 0) return EShaderConstantBuffer::FSubUVConstant;
    if(std::strcmp(str, "FViewportInfo") == 0) return EShaderConstantBuffer::FViewportInfo;
#endif
    return EShaderConstantBuffer::EShaderConstantBuffer_MAX;
}

