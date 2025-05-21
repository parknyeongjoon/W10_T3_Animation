#pragma once

#include "ParticleModule.h"
#include <Distributions/DistributionVector.h>
#include "Distributions/DistributionFloat.h"

class UParticleModuleVelocity : public UParticleModule
{
    DECLARE_CLASS(UParticleModuleVelocity, UParticleModule)
public:
    UParticleModuleVelocity();
    
    /* 초기 속도 */
    FRawDistributionVector StartVelocity;


    /* 방사형 속도 분포 */
    FRawDistributionFloat StartVelocityRadial;

    virtual void Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, float Interp) override;
    virtual void Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime) override;

    virtual EModuleType GetType() const override { return EModuleType::Spawn; } // 또는 Update 필요 시 변경
};