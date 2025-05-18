#pragma once
#include "CoreUObject/UObject/Object.h"
#include "Runtime/Engine/Paritcles/ParticleHelper.h"
#include "CoreUObject/UObject/ObjectMacros.h"
#include "Runtime/Engine/Classes/Particles/ParticleLODLevel.h"

class UParticleLODLevel;

class UParticleEmitter : public UObject
{
    DECLARE_CLASS(UParticleEmitter, UObject);
public:
    UParticleEmitter() {}

    UPROPERTY(
        EditAnywhere,
        TArray<UParticleLODLevel*>,
        LODLevels,
        = TArray<UParticleLODLevel*>();
    )

    void CacheEmitterModuleInfo()
    {
        //ParticleSize = sizeof(FBaseParticle);
    }
};