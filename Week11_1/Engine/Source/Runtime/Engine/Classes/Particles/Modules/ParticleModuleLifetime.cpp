#include "ParticleModuleLifetime.h"
#include "Runtime/Engine/Particles/ParticleEmitterInstances.h"

void UParticleModuleLifeTime::InitializeDefaults()
{
    UDistributionFloatUniform* DefaultDistribution = FObjectFactory::ConstructObject<UDistributionFloatUniform>(nullptr);
    DefaultDistribution->MinValue = 0.0f;
    DefaultDistribution->MaxValue = 1.0f;
    Lifetime.Distribution = DefaultDistribution;
}

void UParticleModuleLifeTime::Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, float Interp)
{
    //BEGIN_UPDATE_LOOP;
    // 수명 값을 설정
    //Particle.Lifetime = Lifetime.GetValue();
    //END_UPDATE_LOOP;
}

EModuleType UParticleModuleLifeTime::GetType() const
{
    return EModuleType::Spawn;

}


