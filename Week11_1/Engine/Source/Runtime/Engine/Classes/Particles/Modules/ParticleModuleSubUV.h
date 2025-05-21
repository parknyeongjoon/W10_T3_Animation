#pragma once
#include "ParticleModule.h"
#include <Distributions/DistributionFloat.h>
#include "Engine/Particles/ParticleHelper.h"

class UParticleModuleSubUV : public UParticleModule {
    DECLARE_CLASS(UParticleModuleSubUV, UParticleModule)
public:
    UParticleModuleSubUV() = default;
    
    //enum class ESubUVMethod
    //{
    //    Random,
    //    Sequential,
    //} Method = ESubUVMethod::Random;
    //
    //FRawDistributionFloat StartImageDistribution;

    // 속도 조절용
    float SubUVSpeed= 10.0f;
public:
    virtual void Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime);
    virtual void Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, float Interp, FBaseParticle* ParticleBase) override;


    virtual EModuleType GetType() const override;

    virtual int32 GetPayloadSize() const override;

};