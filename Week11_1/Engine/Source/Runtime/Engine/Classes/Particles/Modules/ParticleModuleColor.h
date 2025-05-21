#pragma once
#include "ParticleModule.h"
#include "UObject/ObjectMacros.h"
#include "Classes/Distributions/DistributionFloat.h"
#include "Classes/Distributions/DistributionVector.h"

struct FRawDistributionFloat;
struct FRawDistributionVector;
struct FBaseParticle;

class UParticleModuleColor : public UParticleModule
{
    DECLARE_CLASS(UParticleModuleColor, UParticleModule)

public:
    UParticleModuleColor() = default;
    UPROPERTY(
        EditAnywhere,
        FRawDistributionVector,
        StartColor,
    )

    UPROPERTY(
        EditAnywhere,
        FRawDistributionFloat,
        StartAlpha,
    )

    virtual void InitializeDefaults() override;

    virtual void Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, float Interp, FBaseParticle* ParticleBase) override;

    virtual void Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime);

    virtual EModuleType GetType() const override;
};