#pragma once

#include "Particles/Modules/ParticleModule.h"

class UParticleEmitter;
class UParticleSystemComponent;

class UParticleModuleTypeDataBase : public UParticleModule
{
    DECLARE_CLASS(UParticleModuleTypeDataBase, UParticleModule)

public:
    UParticleModuleTypeDataBase() = default;

    virtual FParticleEmitterInstance* CreateInstance(
        UParticleEmitter* InEmitterParent,
        UParticleSystemComponent* InComponent);

    virtual bool IsAMeshEmitter() const { return false; }


};