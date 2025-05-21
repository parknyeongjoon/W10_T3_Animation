#include "ParticleModuleSize.h"
#include "Runtime/Engine/Particles/ParticleEmitterInstances.h"
#include <Particles/ParticleMacros.h>


void UParticleModuleSize::InitializeDefaults()
{
    UDistributionVectorUniform* DefaultDistribution = FObjectFactory::ConstructObject<UDistributionVectorUniform>(nullptr);
    //DefaultDistribution->MinValue = -FVector::OneVector;
    DefaultDistribution->MinValue = FVector(1, 1, 1);
    DefaultDistribution->MaxValue = FVector(10, 10, 10);
    StartSize.Distribution = DefaultDistribution;
}

void UParticleModuleSize::Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, float Interp, FBaseParticle* ParticleBase)
{
     //FIXME : scale의 각 축이 같은 scale을 사용하는 거 구현.
    ParticleBase->Size = StartSize.GetValue();  
    //UE_LOG(LogLevel::Display, "ModuleSize : %f, %f, %f", ParticleBase->Size.X, ParticleBase->Size.Y, ParticleBase->Size.Z);
}

void UParticleModuleSize::Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime)
{
    
}

EModuleType UParticleModuleSize::GetType() const
{
    return EModuleType();
}
