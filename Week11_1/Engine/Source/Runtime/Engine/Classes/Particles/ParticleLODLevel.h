#pragma once
#include "Container/Array.h"
#include "CoreUObject/UObject/Object.h"
#include "ParticleModuleRequired.h"

class UParticleLODLevel : public UObject
{
    int32 Level;
    bool bEnabled;

    UParticleModuleRequired* RequiredModule;
    TArray<UParticleModule*> Modules;
    UParticleModuleTypeDataBase* TypeDataModule;
};