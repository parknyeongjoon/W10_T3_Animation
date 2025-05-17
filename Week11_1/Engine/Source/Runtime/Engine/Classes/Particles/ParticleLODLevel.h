#pragma once
#include "Container/Array.h"
#include "CoreUObject/UObject/Object.h"

class UParticleModule;
class UParticleModuleRequired;
struct UParticleModuleTypeDataBase;

class UParticleLODLevel : public UObject
{
    int32 Level;
    bool bEnabled;

    UParticleModuleRequired* RequiredModule;
    TArray<UParticleModule*> Modules;
    UParticleModuleTypeDataBase* TypeDataModule;
};
