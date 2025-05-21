#pragma once

#include "ParticleModule.h"
#include "Classes/Distributions/DistributionFloat.h"
#include "Classes/Distributions/DistributionVector.h"

struct FBurstInfo
{
    float Time = 0.0f; // EmitterTime 기준
    int Count = 1;     // 생성할 파티클 수
    bool bFired = false;
};

struct FBaseParticle;
struct FParticleEmitterInstance;
class UParticleModuleSpawn : public UParticleModule
{
    DECLARE_CLASS(UParticleModuleSpawn, UParticleModule)

public:
    UParticleModuleSpawn();
    // Property 스폰

    UPROPERTY(
        EditAnywhere,
        FRawDistributionFloat,
        Rate,
        = {}
    )

    UPROPERTY(
        EditAnywhere,
        FRawDistributionFloat,
        RateScale,
        = {}
    )

    // == Burst 관련 ==
    // FIXME : Burst 관련 구현 중단.
    TArray<FBurstInfo> BurstList;


    // Property :: 파티클 시스템
    float UpdateTimeFPS = 60.0f;

    bool bHasSpawnedOnce = false;
public:
    void InitializeDefaults();

    // Tick 마다 스폰 수를 계산
    int32 ComputeSpawnCount(float DeltaTime);

    virtual EModuleType GetType() const override;

    virtual void Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime) override;
    
        // == Burst 관련 ==
    void CheckBurst(FParticleEmitterInstance* Owner, float DeltaTime);

    void ResetBurst();
private:
    float SpawnRemainder = 0.0f;

};

// FIXING : FPropertyChangeEvent& PropertyChangeEvent 인자
    //virtual void PostEditChangeProperty() override;


// FIXING : 커스텀 페이로드 적용 시 필요할 듯 추후 충돌 구현 시 확인
//virtual int32 RequiredBytesPerInstance() const override { return 0; }