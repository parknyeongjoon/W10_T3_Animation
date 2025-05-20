#pragma once
#include "CoreUObject/UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "Runtime/Engine/Paritcles/ParticleHelper.h"

class UParticleLODLevel;

class UParticleEmitter : public UObject
{
    DECLARE_CLASS(UParticleEmitter, UObject)
public:
    UParticleEmitter();
    
    UPROPERTY(EditAnywhere, TArray<UParticleLODLevel*>, LODLevels, = {})


    void CacheEmitterModuleInfo()
    {
        //ParticleSize = sizeof(FBaseParticle);
    }
};

inline UParticleEmitter::UParticleEmitter()
{
}
