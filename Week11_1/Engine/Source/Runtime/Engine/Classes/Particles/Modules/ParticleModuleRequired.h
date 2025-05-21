#pragma once

#include "ParticleModule.h"
#include <Math/Vector.h>
#include <Math/Rotator.h>

#include "Components/PrimitiveComponents/ParticleSystemComponent.h"
#include "UObject/ObjectMacros.h"

class UTexture;
class UStaticMesh;
struct FDynamicSpriteEmitterDataBase;
struct FDynamicSpriteEmitterReplayDataBase;
struct FBaseParticle;

class UParticleModuleRequired : public UParticleModule
{
    DECLARE_CLASS(UParticleModuleRequired, UParticleModule)
public:
    UParticleModuleRequired();
   
    // Property : 이미터
    UPROPERTY(EditAnywhere, UTexture*, Texture, = nullptr)
    UPROPERTY(EditAnywhere, UStaticMesh*, Mesh, = nullptr)

    UPROPERTY(EditAnywhere, FVector, EmitterOrigin, = FVector::ZeroVector)
    UPROPERTY(EditAnywhere, FRotator, EmitterRotation, = FRotator::ZeroRotator)

    // Property : 경과시간
    UPROPERTY(EditAnywhere, bool, EmitterDurationUseRange, = false)
    UPROPERTY(EditAnywhere, bool, EmitterLoops, = false)
    UPROPERTY(EditAnywhere, float, EmitterDuration, = 1.0f)
    UPROPERTY(EditAnywhere, float, EmitterDurationLow, = 0.0f)

    // Property : 딜레이
    UPROPERTY(EditAnywhere, bool, EmitterDelay, = false)
    UPROPERTY(EditAnywhere, bool, EmitterDelayLow, = false)
    // bool DelayFirstLoopOnly;

    UPROPERTY(EditAnywhere, bool, EmitterDelayUseRange, = false)

    // Property : 서브 UV; 
    // InterpolationMethod : Linear;
    UPROPERTY(EditAnywhere, int, SubImagesHorizontal, = 1)
    UPROPERTY(EditAnywhere, int, SubImagesVertical, = 1)

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