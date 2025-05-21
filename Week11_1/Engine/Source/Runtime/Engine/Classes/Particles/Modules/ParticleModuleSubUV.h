#pragma once
#include "ParticleModule.h"

class UParticleModuleSubUV : public UParticleModule {
    DECLARE_CLASS(UParticleModuleSubUV, UParticleModule)
public:
    UParticleModuleSubUV() = default;
    virtual void Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime);

    virtual EModuleType GetType() const override;
};