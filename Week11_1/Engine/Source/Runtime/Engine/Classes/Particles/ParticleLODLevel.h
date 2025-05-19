#pragma once
#include "Container/Array.h"
#include "CoreUObject/UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "Core/HAL/PlatformType.h"

class UParticleModuleRequired;
class UParticleModuleTypeDataBase;
class UParticleModule;

class UParticleLODLevel : public UObject
{
    DECLARE_CLASS(UParticleLODLevel, UObject)
public:

    UParticleLODLevel();

    UParticleModuleRequired* RequiredModule = nullptr;

    // 메시, 빔, 리본 등 Emitter 타입 별 특성 데이터 정의.
    UParticleModuleTypeDataBase* TypeDataModule = nullptr;

    TArray<UParticleModule*> Modules;

    TArray<UParticleModule*> SpawnModules;
    TArray<UParticleModule*> UpdateModules;

    // NOTICE : LOD Level 0만 사용하므로 고정 값 사용.
    int32 Level = 0;
    bool bEnabled = true;

public:
    // Modueles -> SpawnModules, UpdateModules로 분리
    void AnalyzeModules();
};