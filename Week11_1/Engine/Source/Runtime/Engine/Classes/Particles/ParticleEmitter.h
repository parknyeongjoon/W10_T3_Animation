#pragma once
#include "CoreUObject/UObject/Object.h"
#include "Runtime/Engine/Particles/ParticleHelper.h"
#include "CoreUObject/UObject/ObjectMacros.h"
#include <Math/Color.h>
#include "Engine/Classes/Particles/ParticleLODLevel.h"
class UParticleLODLevel;
class UParticleModule;
class UParticleEmitter : public UObject
{
DECLARE_CLASS(UParticleEmitter, UObject)
public:
    UParticleEmitter() = default;
    FName EmitterName;

    FColor EditorColor;

    UPROPERTY(
        EditAnywhere,
        TArray<UParticleLODLevel*>,
        LODLevels,
        = TArray<UParticleLODLevel*>();
    )

    // 런타임 생성되는 하나의 FBaseParticle이 차지하는 메모리 크기
    int32 ParticleSize;

public:
    void CacheEmitterModuleInfo()
    {
        //ParticleSize = sizeof(FBaseParticle);
    }

    //void GatherModules(TArray<UParticleModule*>& OutModules);

    UParticleLODLevel* GetLODLevel(int level);
};