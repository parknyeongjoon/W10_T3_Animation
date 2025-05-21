#pragma once

#pragma once
#include "Core/HAL/PlatformType.h"
#include "ParticleHelper.h"
#include "Core/Math/Rotator.h"
#include "Core/Math/Vector.h"

class UParticleEmitter;
class UParticleSystemComponent;
class UParticleLODLevel;
class UParticleModuleRequired;
class UParticleModuleTypeDataMesh;
class UTexture;

struct FParticleEmitterInstance
{
    UParticleEmitter* SpriteTemplate;
    UParticleSystemComponent* Component;            // Owner

    int32 CurrentLODLevelIndex;
    UParticleLODLevel* CurrentLODLevel = nullptr;

    // === 파티클 데이터 메모리 ===
    uint8* ParticleData = nullptr;                          // FBaseParticle raw memory block
    uint16* ParticleIndices;                                // 인덱스 배열

    int32 ParticleSize = sizeof(FBaseParticle);             // 실제 파티클 한 개 크기
    int32 ParticleStride = sizeof(FBaseParticle);           // 구조체 간 stride

    int32 ActiveParticles;                                  // 현재 살아있는 파티클 수
    int32 MaxActiveParticles;                               // 최대 허용 파티클 수
    uint32 ParticleCounter;                                 // 전체 파티클 생성 수

    // 누적된 시뮬레이션 시간
    float EmitterTime = 0.0f;
    float LoopCount = 0.0f;           // 반복 횟수 추적
    // Required Module 참조 , 나머지 모듈은 LODLevel 배열 순회하여 호출
    UParticleModuleRequired* RequiredModule = nullptr;

    //  === Payload ===
    uint8* InstanceData = nullptr;                                    //
    int32 InstancePayloadSize = 0;
    int32 PayloadOffset = 0;

    FParticleDataContainer DataContainer;

    UTexture* Texture;

public:
    virtual void Init(int32 InMaxparticles);

    virtual void InitParameters(UParticleEmitter* InTemplate, UParticleSystemComponent* InComponent);

    void Release();

    void SpawnParticles(int32 Count, float StartTime, float Increment, const FVector& InitialLocation, const FVector& InitialVelocity);

    virtual FDynamicEmitterDataBase* GetDynamicData() { return nullptr; }

    void KillParticle(int32 Index);

    // == Required Module 설정값 접근 메서드 == 
    UTexture* GetTexture() const;
    UStaticMesh* GetMesh() const;
    FVector GetEmitterOrigin() const;
    FRotator GetEmitterRotation() const;
    int32 GetSubImageH() const;
    int32 GetSubImageV() const;

    void UpdateParticles(float DeltaTime);

protected:
    virtual bool FillReplayData(FDynamicEmitterReplayDataBase& OutData);
};

// Sprite가 기본값 : 사실상 FParticleEmitterInstance와 동일
struct FParticleSpriteEmitterInstance : public FParticleEmitterInstance
{
    virtual FDynamicEmitterDataBase* GetDynamicData() override;

protected:
    virtual bool FillReplayData(FDynamicEmitterReplayDataBase& OutData);
};

struct FParticleMeshEmitterInstance : public FParticleEmitterInstance
{
    UParticleModuleTypeDataMesh* MeshTypeData = nullptr;
    //TArray<UMaterial*> CurrentMaterials;

public:
    virtual void Init(int32 InMaxParticles) override;

    virtual void InitParameters(UParticleEmitter* InTemplate, UParticleSystemComponent* InComponent) override;

    virtual FDynamicEmitterDataBase* GetDynamicData() override;

protected:
    virtual bool FillReplayData(FDynamicEmitterReplayDataBase& OutData);
};

/* void SpawnParticles(int32 Count, float StartTime, float Increment, const FVector& InitialLocation, const FVector& InitialVelocity, struct FParticleEventInstancePayload* EventPayload)
    {
        for (int32 i = 0; i < Count; i++)
        {
            DECLARE_PARTICLE_PTR(Particle, ParticleData + (i * ParticleStride));
            PreSpawn(Particle, InitialLocation, InitialVelocity);

            for (int32 ModuleIndex = 0; ModuleIndex < LODLevel->SpawnModules.Num(); ModuleIndex++)
            {

            }

            PostSpawn(Particle, Interp, SpawnTime);
        }
    }*/