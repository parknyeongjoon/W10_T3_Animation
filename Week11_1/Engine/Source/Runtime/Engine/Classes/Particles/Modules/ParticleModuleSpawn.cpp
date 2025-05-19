#include "ParticleModuleSpawn.h"
#include "Runtime/Engine/Particles/ParticleEmitterInstance.h"

void UParticleModuleSpawn::InitializeDefaults()
{
    if (Rate.Constant <= 0.0f)
    {
        Rate.Constant = 20.0f;
    }

    if ( RateScale.Constant <= 0.0f)
    {
        RateScale.Constant = 1.0f;
    }
}

int32 UParticleModuleSpawn::ComputeSpawnCount(float DeltaTime) const
{
    // Rate와 RateScale의 실제 값을 가져옴
    float RateValue = Rate.GetValue();
    float ScaleValue = RateScale.GetValue();

    float EffectiveRate = RateValue * ScaleValue;

    // 실제 생성해야 하는 파티클 수 = (초당 Rate) * 시간
    float ParticlesToSpawn = EffectiveRate * DeltaTime;

    return int32(ParticlesToSpawn);
}

void UParticleModuleSpawn::Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, float Interp)
{
}

EModuleType UParticleModuleSpawn::GetType() const
{
    return EModuleType::Spawn;
}

 