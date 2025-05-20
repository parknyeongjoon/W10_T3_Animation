#include "ParticleModuleLocation.h"

void UParticleModuleLocation::InitializeDefaults()
{
    UDistributionVectorUniform* DefaultDistribution = FObjectFactory::ConstructObject<UDistributionVectorUniform>(nullptr);
    DefaultDistribution->MinValue = -FVector::OneVector;
    DefaultDistribution->MaxValue = FVector::OneVector;
    StartLocation.Distribution = DefaultDistribution;
}

void UParticleModuleLocation::Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime)
{
}

void UParticleModuleLocation::Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, float Interp)
{
}
