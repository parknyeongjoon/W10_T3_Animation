#pragma once

#include "ParticleModule.h"
#include <Distributions/DistributionVector.h>
#include "Distributions/DistributionFloat.h"

struct FBaseParticle;
class UParticleModuleVelocity : public UParticleModule
{
    DECLARE_CLASS(UParticleModuleVelocity, UParticleModule)
public:
    UParticleModuleVelocity();
    
    /* 초기 속도 */
    UPROPERTY(
        EditAnywhere,
        FRawDistributionVector,
        StartVelocity,
        = FRawDistributionVector();
    )

    /* 방사형 속도 분포의 크기 */
    UPROPERTY(
        EditAnywhere,
        FRawDistributionFloat,
        StartVelocityRadial,
        = FRawDistributionFloat();
    )

    virtual void Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, float Interp, FBaseParticle* ParticleBase) override;

    virtual EModuleType GetType() const override;
};