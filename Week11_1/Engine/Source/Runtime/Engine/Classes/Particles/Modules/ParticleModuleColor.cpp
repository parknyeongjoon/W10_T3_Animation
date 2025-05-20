#include "ParticleModuleColor.h"

void UParticleModuleColor::InitializeDefaults()
{
    UDistributionFloatUniform* DefaultDistributionFloat = FObjectFactory::ConstructObject<UDistributionFloatUniform>(nullptr);
    DefaultDistributionFloat->MinValue = 0.0f;
    DefaultDistributionFloat->MaxValue = 1.0f;
    StartAlpha.Distribution = DefaultDistributionFloat;
    for (int i = 0; i < 10; i++)
    {
        float t = StartAlpha.GetValue();
        t = t + 1;
    }

    UDistributionVectorUniform* DefaultDistributionVector = FObjectFactory::ConstructObject<UDistributionVectorUniform>(nullptr);
    DefaultDistributionVector->MinValue = FVector::ZeroVector;
    DefaultDistributionVector->MaxValue = FVector::OneVector;
    StartColor.Distribution = DefaultDistributionVector;
}

void UParticleModuleColor::Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, float Interp)
{
}

EModuleType UParticleModuleColor::GetType() const
{
    return EModuleType();
}
