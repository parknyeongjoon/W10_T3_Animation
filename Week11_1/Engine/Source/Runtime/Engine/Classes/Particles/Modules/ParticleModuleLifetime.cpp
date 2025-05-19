#include "ParticleModuleLifetime.h"
#include "Runtime/Engine/Particles/ParticleEmitterInstance.h"

void UParticleModuleLifeTime::InitializeDefaults()
{
    //if (Lifetime.Constant <= 0.0f)
    //{
    //    Lifetime.Constant = 1.0f; // 기본 수명 1초
    //}
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


