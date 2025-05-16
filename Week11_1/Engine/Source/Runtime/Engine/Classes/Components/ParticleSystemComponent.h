#pragma once
#include "Container/Array.h"
#include "Particles/ParticleSystem.h"

class UParticleSystemComponent
{
    TArray<struct FParticleEmitterInstance*> EmitterInstances;
    UParticleSystem* Template;

    TArray<FDynamicEmitterDataBase*> EmitterRenderData;
};