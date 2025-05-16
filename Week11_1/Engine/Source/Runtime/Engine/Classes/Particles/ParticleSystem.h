#pragma once
#include "Container/Array.h"
#include "CoreUObject/UObject/Object.h"
#include "ParticleEmitter.h"

class UParticleSystem : public UObject
{
    TArray<UParticleEmitter*> Emitters;

};
