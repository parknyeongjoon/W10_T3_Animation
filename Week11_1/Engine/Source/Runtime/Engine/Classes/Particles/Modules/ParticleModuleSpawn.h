#pragma once

#include "ParticleModule.h"
#include "Classes/Distributions/Distributions.h"

class UParticleModuleSpawn : public UParticleModule
{
    DECLARE_CLASS(UParticleModuleSpawn, UParticleModule)

public:
    UParticleModuleSpawn();
    // Property 스폰
    FRawDistributionFloat Rate;

    FRawDistributionFloat RateScale;

    // Property :: 파티클 시스템
    float UpdateTimeFPS = 60.0f;


public:
    void InitializeDefaults();

    // Tick 마다 스폰 수를 계산
    int32 ComputeSpawnCount(float DeltaTime) const;

    // Spawn 시점에 처리
    virtual void Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, float Interp) override;

    virtual EModuleType GetType() const override;
    
};

// FIXING : FPropertyChangeEvent& PropertyChangeEvent 인자
    //virtual void PostEditChangeProperty() override;


// FIXING : 커스텀 페이로드 적용 시 필요할 듯 추후 충돌 구현 시 확인
//virtual int32 RequiredBytesPerInstance() const override { return 0; }