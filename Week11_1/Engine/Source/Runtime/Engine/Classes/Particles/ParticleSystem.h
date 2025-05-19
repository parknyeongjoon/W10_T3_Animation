#pragma once
#include "Container/Array.h"
#include "CoreUObject/UObject/Object.h"
#include "CoreUObject/UObject/ObjectMacros.h"
#include "ParticleEmitter.h"

class UParticleSystem : public UObject
{
    DECLARE_CLASS(UParticleSystem, UObject);

public:
    UParticleSystem() {}
    TArray<UParticleEmitter*> Emitters;

};
