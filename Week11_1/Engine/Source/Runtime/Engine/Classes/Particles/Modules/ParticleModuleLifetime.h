#pragma once

#include "ParticleModule.h"
#include "Classes/Distributions/DistributionFloat.h"

class UParticleModuleLifeTime : public UParticleModule
{
DECLARE_CLASS(UParticleModuleLifeTime, UParticleModule)
public :
    // 파티클 수명
    UPROPERTY(
        EditAnywhere,
        FRawDistributionFloat,
        Lifetime,
        = {}
    )

public:
    UParticleModuleLifeTime() = default;

    virtual void InitializeDefaults() override;
    
    virtual void Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, float Interp) override;

    virtual EModuleType GetType() const override;
    
};