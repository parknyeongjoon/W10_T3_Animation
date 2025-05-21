#include "ParticleModuleLifetime.h"
#include "Runtime/Engine/Particles/ParticleEmitterInstances.h"
#include <Particles/ParticleMacros.h>
#include "Core/Math/RandomStream.h"
#include "Particles/Modules/ParticleModuleDefaults.h"

void UParticleModuleLifeTime::InitializeDefaults()
{
    
    UDistributionFloatConstant* DefaultDistribution = FObjectFactory::ConstructObject<UDistributionFloatConstant>(nullptr);
    DefaultDistribution->SetValue(ParticleModuleDefaults::Lifetime::Constant);
    Lifetime.Distribution = DefaultDistribution;
}

void UParticleModuleLifeTime::Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, float Interp, FBaseParticle* ParticleBase)
{
    ParticleBase->Lifetime = Lifetime.GetValue();
    UE_LOG(LogLevel::Display, "Lifetime Value : %f", ParticleBase->Lifetime);
}

EModuleType UParticleModuleLifeTime::GetType() const
{
    return EModuleType::Spawn;

}


