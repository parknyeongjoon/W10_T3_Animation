// Auto-generated constant buffer structures with padding
#pragma once

#include "Math/Vector.h"
#include "Math/Vector4.h"
#include "Math/Matrix.h"
#include "Math/Color.h"
#include "Define.h"

const int MAX_POINTLIGHT = 8;
const int MAX_SPOTLIGHT = 8;

struct alignas(16) FFogCameraConstant
{
    FMatrix InvProjMatrix;
    FMatrix InvViewMatrix;

    FVector CameraPos;
    float NearPlane;
    FVector CameraForward;
    float FarPlane;
};

// NOTE: Generated code - do not modify manually.
struct alignas(16) FMeshConstant
{
    FMatrix ModelMatrix;
    FMatrix ModelInvTransMatrix;
    FVector DiffuseColor;
    float TransparencyScalar;
    FVector AmbientColor;
    float DensityScalar;
    FVector SpecularColor;
    float SpecularScalar;
    FVector EmmisiveColor;
    float MaterialPad0;
    uint32 IsSelectedMesh;
};

struct alignas(16) FSceneConstant
{
    FMatrix ViewMatrix;
    FMatrix ProjMatrix;
    alignas(16) FVector CameraPos;
    alignas(16) FVector CameraLookAt;
};

struct alignas(16) FDebugIconConstant
{
    FVector IconPosition;
    float IconScale;
};

struct alignas(16) FDebugArrowConstant
{
    FVector ArrowPosition;
    float ArrowScaleXYZ;
    FVector ArrowDirection;
    float ArrowScaleZ;
};

struct alignas(16) FCameraConstant
{
    FMatrix ViewMatrix; // offset: 0, size: 64
    FMatrix ProjMatrix; // offset: 64, size: 64
    FMatrix ViewProjMatrix; // offset: 128, size: 64
    FVector CameraPos; // offset: 192, size: 12
    float NearPlane; // offset: 204, size: 4
    FVector CameraForward; // offset: 208, size: 12
    float FarPlane; // offset: 220, size: 4
};

struct alignas(16) FViewportInfo
{
    FVector2D ViewportSize; // offset: 0, size: 8
    FVector2D ViewportOffset; // offset: 8, size: 8
};

struct alignas(16) FFogParams
{
    FVector FogColor;
    float FogDensity;
    float FogStart;
    float FogEnd;
    float FogZPosition;
    float FogBaseHeight;
    float HeightFallOff;
    int bIsHeightFog;
    float FogMaxOpacity; // MaxOpacity 추가
    float LightShaftDensity; // 추가: 광선 밀도 [4]
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
    int SphereCount; // offset: 4, size: 4
    int ConeCount; // offset: 8, size: 4
    int LineCount; // offset: 12, size: 4
    int CapsuleCount;
    int OBBCount;
    float pad1;
    float pad2;
};

struct alignas(16) FSubUVConstant
{
    float indexU; // offset: 0, size: 4
    float indexV; // offset: 4, size: 4
    FVector2D Pad; // offset: 8, size: 8
};

struct alignas(16) FConstants
{
    FMatrix MVP; // offset: 0, size: 64
    float Flag; // offset: 64, size: 4
    uint8 pad0[12]; // Padding to end of buffer
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
    uint32 bHasNormalTexture; // offset: 60, size: 4
};

struct alignas(16) FLightingConstants
{
    uint32 NumPointLights; // offset: 4, size: 4
    uint32 NumSpotLights;
    float pad; // offset: 8, size: 8
    float pad2;
    FDirectionalLight DirLight;
    FPointLight PointLights[MAX_POINTLIGHT];
    FSpotLight SpotLights[MAX_SPOTLIGHT]; 
};

struct alignas(16) FFadeConstants
{
    FLinearColor FadeColor = {0.f,0.f,0.f,1.f}; // 페이드 목표 색상 (예: 검은색 float4(0,0,0,1))
    
    float FadeAlpha = 0.0f;       // 보간 계수 (0.0 = SceneColor, 1.0 = FadeColor)
    float Padding;   
    float Padding1;   
    float Padding2;   
};

struct alignas(16)  FLetterBoxConstants // 레지스터 슬롯은 다른 버퍼와 겹치지 않게 선택 (b0가 Fade에 사용되었다면 b1 사용)
{
    // 레터박스 바의 색상 (보통 검은색 float4(0,0,0,1))
    FLinearColor LetterboxColor = {0.f,0.f,0.f,1.f};
    
    float  LetterboxSize = 0.0f; 
    float  PillarboxSize = 0.0f; 
    float Padding;   
    float Padding2;  
    
};

struct alignas(16) FBlurConstants
{
    // 블러 강도 (가우시안 함수의 표준 편차(sigma) 역할)
    float BlurStrength;
    float TexelSizeX;
    float TexelSizeY;
    float Padding;
};

struct alignas(16) FFlagConstants
{
    uint32 IsLit; // offset: 0, size: 4
    uint32 IsNormal;
    uint32 IsVSM;
    uint32 IsGPUSkinning;
};

struct alignas(16) FMatrixConstants
{
    FMatrix Model; // offset: 0, size: 64
    FMatrix ViewProj; // offset: 64, size: 64
    FMatrix MInverseTranspose; // offset: 128, size: 64
    bool isSelected; // offset: 192, size: 4
    FVector pad0; // offset: 196, size: 12
};

struct alignas(16) FMatrixSeparatedMVPConstants
{
    FMatrix Model;
    FMatrix View;
    FMatrix Proj;
    FMatrix InvView;
};

struct alignas(16) FConstatntBufferActor
{
    FVector4 UUID; // offset: 0, size: 16
    uint32 IsSelectedActor; // offset: 16, size: 4
    FVector padding; // offset: 20, size: 12
};

struct alignas(16) FBoneConstant
{
    FMatrix BoneSkinningMatrices[128];
};

struct alignas(16) FParticleConstant
{
    float Alpha;
    FVector ParticlePadding;
};

