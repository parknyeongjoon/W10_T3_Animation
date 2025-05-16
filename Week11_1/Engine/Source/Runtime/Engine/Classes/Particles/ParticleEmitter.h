#pragma once
#include "CoreUObject/UObject/Object.h"
#include "Runtime/Engine/Paritcles/ParticleHelper.h"

class UParticleLODLevel;

class UParticleEmitter : public UObject
{
    TArray<UParticleLODLevel*> LODLevels;


    void CacheEmitterModuleInfo()
    {
        //ParticleSize = sizeof(FBaseParticle);
    }
};