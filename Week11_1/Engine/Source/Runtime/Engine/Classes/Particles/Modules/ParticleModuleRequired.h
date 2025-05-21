#pragma once

#include "ParticleModule.h"
#include <Math/Vector.h>
#include <Math/Rotator.h>
#include "UObject/ObjectMacros.h"

struct FTexture;
struct FDynamicSpriteEmitterDataBase;
struct FDynamicSpriteEmitterReplayDataBase;
struct FBaseParticle;

class UParticleModuleRequired : public UParticleModule
{
    DECLARE_CLASS(UParticleModuleRequired, UParticleModule)
public:
    UParticleModuleRequired();
   
    // Property : 이미터
    FTexture* Texture;
    FVector EmitterOrigin;
    FRotator EmitterRotation;

    // Property : 경과시간
    bool EmitterDurationUseRange;
    float EmitterLoops;
    float EmitterDuration;
    float EmitterDurationLow;

    // Property : 딜레이
    float EmitterDelay;
    float EmitterDelayLow;
    // bool DelayFirstLoopOnly;
    bool EmitterDelayUseRange;

    // Property : 서브 UV; 
    // InterpolationMethod : Linear;
    int SubImagesHorizontal;
    int SubImagesVertical;

public:

    void InitializeDefaults();

    void CacheEmitterData(FParticleEmitterInstance* Instance) const;

    void FillReplayData(FDynamicSpriteEmitterReplayDataBase* ReplayData) const;

    int32 GetTotalSubImages() const;

    float ComputeEmitterDuration(float RandomFactor) const;

    virtual EModuleType GetType() const override;
    
    virtual void Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime) override;

    virtual void PostEditChangeProperty() override;
};


//enum class ESubUVInterpolationMethod
//{
//    None,
//    Linear,
//    Random
//} InterpolationMethod = ESubUVInterpolationMethod::Linear;
//
///** 기타 필수 설정 (추가 예정 필드들) */
//enum class EScreenAlignment
//{
//    PSA_Square,
//    PSA_Velocity,
//    PSA_FacingCameraPosition,
//    PSA_FacingCameraDistanceBlend,
//    // ...
//};
//
//EScreenAlignment ScreenAlignment = EScreenAlignment::PSA_Square;
//
//enum class EParticleBlendMode
//{
//    Opaque,
//    Additive,
//    Translucent
//};
//
//EParticleBlendMode BlendMode = EParticleBlendMode::Additive;
//
//// 필요 시 추가: SortMode, bUseLocalSpace 등