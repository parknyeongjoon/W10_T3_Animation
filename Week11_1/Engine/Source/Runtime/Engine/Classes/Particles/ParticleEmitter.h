#pragma once
#include "CoreUObject/UObject/Object.h"
#include "Runtime/Engine/Particles/ParticleHelper.h"
#include "CoreUObject/UObject/ObjectMacros.h"
#include <Math/Color.h>
//#include "Engine/Classes/Particles/ParticleLODLevel.h" // UProperty 때문에 전방선언 불가

struct FParticleEmitterInstance;
class UParticleSystemComponent;
class UParticleLODLevel;

// !!! 가상 클래스. UParticleSpriteEmitter를 사용하기
// CreateInstance() 함수는 UParticleSpriteEmitter만 작동.
// Module중에 UParticleModuleTypeDataBase가 존재하면 그 모듈에 맞춰서 인스턴스를 생성
// 없을경우 스프라이트 생성
class UParticleEmitter : public UObject
{
    DECLARE_CLASS(UParticleEmitter, UObject)
public:
    UParticleEmitter() = default;
    UPROPERTY(EditAnywhere, FName, EmitterName, = NAME_None)

    UPROPERTY(EditAnywhere, FColor, EditorColor, = FColor())

    UPROPERTY(
        EditAnywhere,
        TArray<UParticleLODLevel*>,
        LODLevels,
        = TArray<UParticleLODLevel*>();
    )

    // 런타임 생성되는 하나의 FBaseParticle이 차지하는 메모리 크기
    UPROPERTY(EditAnywhere, int32, ParticleSize, = 0)

public:
    // 
    virtual FParticleEmitterInstance* CreateInstance(UParticleSystemComponent* InComponent);

    void CacheEmitterModuleInfo()
    {
        //ParticleSize = sizeof(FBaseParticle);
    }

    //void GatherModules(TArray<UParticleModule*>& OutModules);

    UParticleLODLevel* GetLODLevel(int level);
};

// 기본값
class UParticleSpriteEmitter : public UParticleEmitter
{
    DECLARE_CLASS(UParticleSpriteEmitter, UParticleEmitter)

public:
    UParticleSpriteEmitter() = default;

    virtual FParticleEmitterInstance* CreateInstance(UParticleSystemComponent* InComponent) override;
};