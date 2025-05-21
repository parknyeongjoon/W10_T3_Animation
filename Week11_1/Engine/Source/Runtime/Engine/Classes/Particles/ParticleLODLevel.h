#pragma once
#include "Container/Array.h"
#include "CoreUObject/UObject/Object.h"
#include "CoreUObject/UObject/ObjectMacros.h"
#include "Core/HAL/PlatformType.h"

class UParticleModule;
class UParticleModuleRequired;
class UParticleModuleTypeDataBase;

class UParticleLODLevel : public UObject
{
    DECLARE_CLASS(UParticleLODLevel, UObject)
public:
    UParticleLODLevel() {}

    UPROPERTY(EditAnywhere, UParticleModuleRequired*, RequiredModule, = nullptr)

    // 메시, 빔, 리본 등 Emitter 타입 별 특성 데이터 정의.
    // 만약 nullptr이면 sprite를 의미
    UPROPERTY(EditAnywhere, UParticleModuleTypeDataBase*, TypeDataModule, = nullptr)

    UPROPERTY(EditAnywhere, TArray<UParticleModule*>, Modules, = {})

    UPROPERTY(EditAnywhere, TArray<UParticleModule*>, SpawnModules, = {})

    UPROPERTY(EditAnywhere, TArray<UParticleModule*>, UpdateModules, = {})

    // NOTICE : LOD Level 0만 사용하므로 고정 값 사용.
    int32 Level = 0;
    bool bEnabled = true;

public:
    // Modueles -> SpawnModules, UpdateModules로 분리
    void AnalyzeModules();
};
