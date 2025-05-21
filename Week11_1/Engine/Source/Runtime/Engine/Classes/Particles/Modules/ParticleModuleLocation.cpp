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

void UParticleModuleLocation::Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, float Interp, FBaseParticle* ParticleBase)
{
    ParticleBase->Location += StartLocation.GetValue();
    return;
}

EModuleType UParticleModuleLocation::GetType() const
{
    return EModuleType::Spawn;
}
