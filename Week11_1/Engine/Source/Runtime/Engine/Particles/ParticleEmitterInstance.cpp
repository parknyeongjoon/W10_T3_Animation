#include "ParticleEmitterInstance.h"
#include <UserInterface/Console.h>
#include "Core/HAL/PlatformMemory.h"
#include "Particles/Modules/ParticleModuleRequired.h"
#include <Math/Rotator.h>
#include "Engine/Particles/ParticleHelper.h"
#include <Engine/Texture.h>

void FParticleEmitterInstance::Init(int32 InMaxParticles)
{
    MaxActiveParticles = InMaxParticles;
    ParticleStride = sizeof(FBaseParticle);
    ParticleSize = ParticleStride;

    const int32 DataSize = MaxActiveParticles * ParticleStride;
    const int32 IndexSize = MaxActiveParticles * sizeof(uint16);

    ParticleData = new uint8[DataSize + IndexSize];
    ParticleIndices = reinterpret_cast<uint16*>(ParticleData + DataSize);

    ActiveParticles = 0;
    ParticleCounter = 0;
}

void FParticleEmitterInstance::Release()
{
    delete[] ParticleData;
    ParticleData = nullptr;
    ParticleIndices = nullptr;
    MaxActiveParticles = 0;
    ActiveParticles = 0;
}

FBaseParticle* FParticleEmitterInstance::GetParticle(int32 Index)
{
    return reinterpret_cast<FBaseParticle*>(ParticleData + Index * ParticleStride);
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
        
    }
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