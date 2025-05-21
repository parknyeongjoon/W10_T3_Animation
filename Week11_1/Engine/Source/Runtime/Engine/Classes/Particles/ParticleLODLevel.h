#pragma once
#include "Container/Array.h"
#include "CoreUObject/UObject/Object.h"

class UParticleModule;
class UParticleModuleRequired;
struct UParticleModuleTypeDataBase;

class UParticleLODLevel : public UObject
{
    DECLARE_CLASS(UParticleLODLevel, UObject)
public:
    UParticleLODLevel() = default;
    ~UParticleLODLevel() override;

    int32 Level = 0;
    bool bEnabled = false;

public:
    UParticleModuleRequired* RequiredModule = nullptr;
    UPROPERTY(EditAnywhere, TArray<UParticleModule*>, Modules, = {})
    UParticleModuleTypeDataBase* TypeDataModule = nullptr;
};

inline UParticleLODLevel::~UParticleLODLevel()
{
}

