#pragma once
#include "Container/Array.h"
#include "CoreUObject/UObject/Object.h"
#include "CoreUObject/UObject/ObjectMacros.h"

class UParticleModule;
class UParticleModuleRequired;
struct UParticleModuleTypeDataBase;

class UParticleLODLevel : public UObject
{
    DECLARE_CLASS(UParticleLODLevel, UObject);
public:
    UParticleLODLevel()
        : Level(0)
        , bEnabled(true)
        , RequiredModule(nullptr)
        , TypeDataModule(nullptr)
    {
    }
    int32 Level;
    bool bEnabled;

    UParticleModuleRequired* RequiredModule;
    TArray<UParticleModule*> Modules;
    UParticleModuleTypeDataBase* TypeDataModule;
};
