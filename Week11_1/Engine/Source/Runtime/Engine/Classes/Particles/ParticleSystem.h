#pragma once
#include "Container/Array.h"
#include "CoreUObject/UObject/Object.h"
#include "ParticleEmitter.h"

class UParticleEmitter;

class UParticleSystem : public UObject
{
    DECLARE_CLASS(UParticleSystem, UObject)

public:
    UParticleSystem() = default;

    // 루프 여부
    bool bLooping;

    // 시스템 전체 수명
    float Duration;

    // 시스템 루프 간 지연 시간
    float Delay;

    TArray<UParticleEmitter*> Emitters;

    // TODO :  이 시스템의 최댓값으로 추정되는 파티클 수 
    //int32 EstimatedMaxActiveParticleCount;

public:
    void InitializeSystem();
    void AddEmitter(UParticleEmitter* Emitter);

    // TODO : 시스템 복제
    //UParticleSystem* DuplicateSystem() const;

    // 디버깅용 이름 반환
    FString GetDebugName() const;

};