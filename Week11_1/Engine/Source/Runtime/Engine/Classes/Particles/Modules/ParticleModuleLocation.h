#pragma once

#include "ParticleModule.h"
#include "Classes/Distributions/DistributionVector.h"

struct FBaseParticle;
class UParticleModuleLocation : public UParticleModule
{
    DECLARE_CLASS(UParticleModuleLocation, UParticleModule)

public:
    UParticleModuleLocation() = default;

    UPROPERTY(
        EditAnywhere,
        FRawDistributionVector,
        StartLocation,
        = {}
    )

    virtual void InitializeDefaults() override;

    virtual void Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime);

    virtual void Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, float Interp, FBaseParticle* ParticleBase) override;
    virtual EModuleType GetType() const override;
};