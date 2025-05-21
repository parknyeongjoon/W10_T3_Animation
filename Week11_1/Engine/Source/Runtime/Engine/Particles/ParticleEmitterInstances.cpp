#include "ParticleEmitterInstances.h"

#include "CoreUObject/UObject/Casts.h"
#include "UserInterface/Console.h"
#include "Core/HAL/PlatformMemory.h"
#include "Particles/Modules/ParticleModuleRequired.h"
#include <Math/Rotator.h>
#include "Engine/Particles/ParticleHelper.h"
#include <Engine/Texture.h>
#include "Classes/Particles/ParticleLODLevel.h"
#include "Particles/TypeData/ParticleModuleTypeDataMesh.h"
#include "ParticleMacros.h"
#include "Particles/ParticleEmitter.h"

void FParticleEmitterInstance::Init(int32 InMaxParticles)
{
    MaxActiveParticles = InMaxParticles;
    ParticleStride = sizeof(FBaseParticle);
    ParticleSize = ParticleStride;

    // Alloc방식으로 변경
    /*const int32 DataSize = MaxActiveParticles * ParticleStride;
    const int32 IndexSize = MaxActiveParticles * sizeof(uint16);

    ParticleData = new uint8[DataSize + IndexSize];
    ParticleIndices = reinterpret_cast<uint16*>(ParticleData + DataSize);*/

    DataContainer.Alloc(MaxActiveParticles * ParticleStride, MaxActiveParticles);

    // 외부 포인터 캐시: 기존 코드와의 호환성을 위해 그대로 유지
    ParticleData = DataContainer.ParticleData;
    ParticleIndices = DataContainer.ParticleIndices;

    ActiveParticles = 0;
    ParticleCounter = 0;
    ParticleCounter = 0;
}

void FParticleEmitterInstance::InitParameters(UParticleEmitter* InTemplate, UParticleSystemComponent* InComponent)
{
    SpriteTemplate = InTemplate;
    Component = InComponent;
}

void FParticleEmitterInstance::Release()
{
    //delete[] ParticleData;

    DataContainer.Free();
    ParticleData = nullptr;
    ParticleIndices = nullptr;
    MaxActiveParticles = 0;
    ActiveParticles = 0;
}

void FParticleEmitterInstance::SpawnParticles(int32 Count, float StartTime, float Increment, const FVector& InitialLocation, const FVector& InitialVelocity)
{
    if (!CurrentLODLevel) {
        UE_LOG(LogLevel::Warning, "[FParticleEmitterInstance] There is no CurrentLODLevel");
    }

    for (int32 i = 0; i < Count; ++i) {
        if (ActiveParticles >= MaxActiveParticles) {
            // 최대 파티클 초과 시 무시
            break;
        }

        uint8* Address = ParticleData + ActiveParticles * ParticleStride;
        DECLARE_PARTICLE_PTR(Particle, Address);

        // == 기본 값 초기화 ==
        Particle->Location = InitialLocation;
        Particle->Velocity = InitialVelocity;
        Particle->BaseVelocity = InitialVelocity;

        Particle->RelativeTime = 0.0f;
        Particle->Lifetime = 10000.0f;

        Particle->Rotation = FVector(0.f, 0.f, 0.f);
        Particle->RotationRate = 0.0f;

        Particle->Size = FVector(1.0f);
        Particle->Color = FColor::White;

        for (UParticleModule* Module : CurrentLODLevel->SpawnModules)
        {
            if (Module)
            {
                float SpawnTime = StartTime + i * Increment;
                Module->Spawn(this, /*Offset*/ 0, SpawnTime, /*Interp*/ 1.0f, Particle);
            }
        }

        ParticleIndices[ActiveParticles] = ActiveParticles;
        ActiveParticles++;
        ParticleCounter++;

    }
}

// 하위 클래스에서 호출되어서 기본적인 값을 넣어줌.
bool FParticleEmitterInstance::FillReplayData(FDynamicEmitterReplayDataBase& OutData)
{
    if (!SpriteTemplate)
    {
        return false;
    }

    if (ActiveParticles <= 0)
    {
        return false;
    }

    UParticleLODLevel* LODLevel = SpriteTemplate->GetLODLevel(CurrentLODLevelIndex);

    OutData.ActiveParticleCount = ActiveParticles;
    OutData.eEmitterType = DET_Unknown;
    OutData.ParticleStride = ParticleStride;
    //OutData.DataContainer : 하위 클래스에서 채워줌
    //OutData.Texture : 하위 클래스에서 채워줌
    return true;
}

void FParticleEmitterInstance::KillParticle(int32 Index)
{
    if (Index < 0 || Index >= ActiveParticles) {
        UE_LOG(LogLevel::Warning, "FParticleEmitterInstance::KillPartilce - Invalid Index");
    }

    const int32 LastIndex = ActiveParticles - 1;

    // 마지막 파티클과 죽일 파티클 스왑
    if (Index != LastIndex) {
        uint8* Dest = ParticleData + Index * ParticleStride;
        uint8* Src = ParticleData + LastIndex * ParticleStride;


        FPlatformMemory::Memcpy(Dest, Src, ParticleStride);
    }

    ActiveParticles--;
}

FTexture* FParticleEmitterInstance::GetTexture() const
{
    return RequiredModule ? RequiredModule->Texture : nullptr;
}

FVector FParticleEmitterInstance::GetEmitterOrigin() const
{
    return RequiredModule ? RequiredModule->EmitterOrigin : FVector::ZeroVector;
}

FRotator FParticleEmitterInstance::GetEmitterRotation() const
{
    return RequiredModule ? RequiredModule->EmitterRotation : FRotator::ZeroRotator;

}

int32 FParticleEmitterInstance::GetSubImageH() const
{
    return RequiredModule ? RequiredModule->SubImagesHorizontal : 1;
}

int32 FParticleEmitterInstance::GetSubImageV() const
{
    return RequiredModule ? RequiredModule->SubImagesVertical : 1;
}

void FParticleEmitterInstance::UpdateParticles(float DeltaTime)
{
    for (int32 i = 0; i < ActiveParticles; ++i)
    {
        uint8* Address = ParticleData + i * ParticleStride;
        DECLARE_PARTICLE_PTR(Particle, Address);
        // FBaseParticle* Particle = GetParticle(i);
        if (!Particle)
            continue;

        // 단순 선형 이동: Velocity 기준 위치 이동
        Particle->Location += Particle->Velocity * DeltaTime;

        // 예시: BaseVelocity를 중력처럼 사용할 경우 (선택)
        // Particle->Velocity += Particle->BaseVelocity * DeltaTime;

        // OldPosition이 필요한 경우 (모션 벡터, Trail 등)
        // Particle->OldLocation = Particle->Location;
    }
}

void FParticleMeshEmitterInstance::Init(int32 InMaxParticles)
{
    FParticleEmitterInstance::Init(InMaxParticles);
}

void FParticleMeshEmitterInstance::InitParameters(UParticleEmitter* InTemplate, UParticleSystemComponent* InComponent)
{
    FParticleEmitterInstance::InitParameters(InTemplate, InComponent);
    MeshTypeData = Cast<UParticleModuleTypeDataMesh>(CurrentLODLevel->TypeDataModule);
}

FDynamicEmitterDataBase* FParticleSpriteEmitterInstance::GetDynamicData()
{
    UParticleLODLevel* LODLevel = SpriteTemplate->GetLODLevel(CurrentLODLevelIndex);
    
    FDynamicSpriteEmitterData* NewEmitterData = new FDynamicSpriteEmitterData();

    if (!FillReplayData(NewEmitterData->Source))
    {
        delete NewEmitterData;
        return nullptr;
    }
    return NewEmitterData;
}

bool FParticleSpriteEmitterInstance::FillReplayData(FDynamicEmitterReplayDataBase& OutData)
{
    if (!FParticleEmitterInstance::FillReplayData(OutData))
    {
        return false;
    }

    OutData.eEmitterType = DET_Sprite;

    FDynamicSpriteEmitterReplayDataBase* NewReplayData =
        static_cast<FDynamicSpriteEmitterReplayDataBase*>(&OutData);

    NewReplayData->Texture = GetTexture();

    NewReplayData->DataContainer.MemBlockSize = MaxActiveParticles * ParticleStride;
    NewReplayData->DataContainer.ParticleData = ParticleData;
    NewReplayData->DataContainer.ParticleIndices = ParticleIndices;
    NewReplayData->SubImages_Horizontal = GetSubImageH();
    NewReplayData->SubImages_Vertical = GetSubImageV();

    return true;
}


FDynamicEmitterDataBase* FParticleMeshEmitterInstance::GetDynamicData()
{
    UParticleLODLevel* LODLevel = SpriteTemplate->GetLODLevel(CurrentLODLevelIndex);

    FDynamicSpriteEmitterData* NewEmitterData = new FDynamicSpriteEmitterData();

    if (!FillReplayData(NewEmitterData->Source))
    {
        delete NewEmitterData;
        return nullptr;
    }

    return NewEmitterData;
}

bool FParticleMeshEmitterInstance::FillReplayData(FDynamicEmitterReplayDataBase& OutData)
{
    if (!FParticleEmitterInstance::FillReplayData(OutData))
    {
        return false;
    }

    OutData.eEmitterType = DET_Mesh;

    //FDynamicMeshEmitterReplayData* NewReplayData =
        //static_cast<FDynamicMeshEmitterReplayData*>(&OutData);

    // 메시 정보는 여기에 저장하지 않음.
    // 식별을 위해서만 DET_Mesh로 설정하고 끝.
}

// FORCEINLINE static void* FastParticleSmallBlockAlloc(size_t AllocSize)
// {
//     if (GEnableFastPools)
//     {
//         FScopeLock S(&GFastPoolsCriticalSection);
//
//         FFastPoolFreePool* Allocations = GFastPoolFreedAllocations.Find(AllocSize);
//         if ( Allocations  && Allocations->FreeAllocations.Num() )
//         {
//             void* Result = Allocations->FreeAllocations[0];
//             Allocations->FreeAllocations.RemoveAtSwap(0, EAllowShrinking::No);
//             Allocations->LastUsedTime = FPlatformTime::Seconds();
//             GFreePoolSizeBytes -= AllocSize;
//             return Result;
//         }
//     }
//
//     return FPlatformMemory::Malloc(AllocSize);
// }

// FORCEINLINE static void FastParticleSmallBlockFree(void *RawMemory, size_t AllocSize)
// {
//     if (GEnableFastPools)
//     {
//         FScopeLock S(&GFastPoolsCriticalSection);
//         FFastPoolFreePool& Allocations = GFastPoolFreedAllocations.FindOrAdd(AllocSize);
//         Allocations.FreeAllocations.Add(RawMemory);
//         GFreePoolSizeBytes += AllocSize;
//
//         if ( GFreePoolSizeBytes > GMaxFreePoolSizeBytes )
//         {
//             // free the oldest allocation
//             FFastPoolFreePool* OldestPool = nullptr;
//             int32 OldestPoolAllocSize  = 0;
//             for ( auto& PoolItr : GFastPoolFreedAllocations )
//             {
//                 FFastPoolFreePool& Pool = PoolItr.Value;
//                 if ( Pool.FreeAllocations.Num() )
//                 {
//                     if ( OldestPool == nullptr )
//                     {
//                         OldestPoolAllocSize = PoolItr.Key;
//                         OldestPool = &Pool;
//                     }
//                     else
//                     {
//                         if (OldestPool->LastUsedTime > Pool.LastUsedTime)
//                         {
//                             OldestPoolAllocSize = PoolItr.Key;
//                             OldestPool = &Pool;
//                         }
//                     }
//                 }
//             }
//             check( OldestPool );
//             check( OldestPoolAllocSize  != 0 );
//             void* OldAllocation = OldestPool->FreeAllocations[0];
//             OldestPool->FreeAllocations.RemoveAtSwap(0, EAllowShrinking::No);
//             GFreePoolSizeBytes -= OldestPoolAllocSize;
//             FMemory::Free(OldAllocation);
//         }
//         return;
//     }
//     FMemory::Free(RawMemory);
// }

