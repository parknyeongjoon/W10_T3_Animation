#include "ParticleModuleLocation.h"
#include "Runtime/Engine/Particles/ParticleEmitterInstances.h"
#include "Core/Math/RandomStream.h"
#include <Particles/ParticleMacros.h>

void UParticleModuleLocation::InitializeDefaults()
{
    UDistributionVectorUniform* DefaultDistribution = FObjectFactory::ConstructObject<UDistributionVectorUniform>(nullptr);
    //DefaultDistribution->MinValue = -FVector::OneVector;
    DefaultDistribution->MinValue = FVector(-10, -10, -10);
    DefaultDistribution->MaxValue = FVector(10, 10, 10);
    StartLocation.Distribution = DefaultDistribution;
}

void UParticleModuleLocation::Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime)
{
}

void UParticleModuleLocation::Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, float Interp)
{
    SPAWN_INIT

    for (int32 i = 0; i < ActiveCount; ++i)
    {
        uint8* Address = ParticleData + i * ParticleStride;
        DECLARE_PARTICLE_PTR(Particle, Address);
        Particle->Location = StartLocation.GetValue();
        UE_LOG(LogLevel::Display, "Location Value : %f, %f, %f", Particle->Location.X, Particle->Location.Y, Particle->Location.Z);
    }
}

EModuleType UParticleModuleLocation::GetType() const
{
    return EModuleType::Spawn;
}
