#include "ParticleModule.h"
#include "Core/HAL/PlatformType.h"
#include "Particles/ParticleEmitterInstances.h"

bool UParticleModule::IsSpawnModule()
{
    return GetType() == EModuleType::Spawn
        || GetType() == EModuleType::Both;
}

bool UParticleModule::IsUpdateModule()
{
    return GetType() == EModuleType::Update
        || GetType() == EModuleType::Both;
}

void UParticleModule::InitializeDefaults()
{
    return;
}

void UParticleModule::Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime)
{
    return;
}

void UParticleModule::Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, float Interp, FBaseParticle* ParticleBase)
{
    return;
}

void UParticleModule::PostEditChangeProperty()
{
}

EModuleType UParticleModule::GetType() const
{
    return EModuleType::Spawn;
}

