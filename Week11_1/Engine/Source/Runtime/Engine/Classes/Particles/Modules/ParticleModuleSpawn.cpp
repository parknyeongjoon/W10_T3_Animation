#include "ParticleModuleSpawn.h"
#include "Runtime/Engine/Particles/ParticleEmitterInstances.h"

UParticleModuleSpawn::UParticleModuleSpawn()
{
    InitializeDefaults();
}

void UParticleModuleSpawn::InitializeDefaults()
{
    UDistributionFloatUniform* DefaultDistribution = FObjectFactory::ConstructObject<UDistributionFloatUniform>(nullptr);
    DefaultDistribution->MinValue = 0.0f;
    DefaultDistribution->MaxValue = 20.0f;
    Rate.Distribution = DefaultDistribution;

    UDistributionFloatUniform* DefaultDistributionScale = FObjectFactory::ConstructObject<UDistributionFloatUniform>(nullptr);
    DefaultDistributionScale->MinValue = 0.0f;
    DefaultDistributionScale->MaxValue = 1.0f;
    RateScale.Distribution = DefaultDistributionScale;
}

int32 UParticleModuleSpawn::ComputeSpawnCount(float DeltaTime)
{
    // Rate와 RateScale의 실제 값을 가져옴
    float RateValue = Rate.GetValue();
    float ScaleValue = RateScale.GetValue();

    float EffectiveRate = RateValue * ScaleValue;

    // 실제 생성해야 하는 파티클 수 = (초당 Rate) * 시간
    float ParticlesToSpawn = EffectiveRate * DeltaTime + SpawnRemainder;

    int32 IntegerPart = static_cast<int32>(ParticlesToSpawn);
    
    SpawnRemainder = ParticlesToSpawn - IntegerPart;

    return IntegerPart;
}

void UParticleModuleSpawn::Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, float Interp)
{
}

EModuleType UParticleModuleSpawn::GetType() const
{
    return EModuleType::Spawn;
}

 