#include "ParticleModuleLifetime.h"
#include "Runtime/Engine/Particles/ParticleEmitterInstances.h"
#include <Particles/ParticleMacros.h>
#include "Core/Math/RandomStream.h"
#include "Particles/Modules/ParticleModuleDefaults.h"

void UParticleModuleLifeTime::InitializeDefaults()
{
    // FIXME : 테스트용 uniform 타입 생성 기본값으로 변경.
    UDistributionFloatUniform* DefaultDistribution = FObjectFactory::ConstructObject<UDistributionFloatUniform>(nullptr);
    DefaultDistribution->MinValue = ParticleModuleDefaults::Lifetime::Min;
    DefaultDistribution->MaxValue = ParticleModuleDefaults::Lifetime::Max;
    Lifetime.Distribution = DefaultDistribution;
}

void UParticleModuleLifeTime::Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, float Interp, FBaseParticle* ParticleBase)
{
    ParticleBase->Lifetime = Lifetime.GetValue();
    //UE_LOG(LogLevel::Display, "Lifetime Value : %f", ParticleBase->Lifetime);
}

EModuleType UParticleModuleLifeTime::GetType() const
{
    return EModuleType::Spawn;

}


