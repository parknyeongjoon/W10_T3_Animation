#include "ParticleModuleVelocity.h"
#include "Distributions//DistributionFloat.h"
#include "Distributions/DistributionVector.h"
#include "Particles/ParticleEmitterInstances.h"
#include <Particles/ParticleMacros.h>
#include "Particles/Modules/ParticleModuleDefaults.h"

UParticleModuleVelocity::UParticleModuleVelocity()
{
    StartVelocity.Distribution = FObjectFactory::ConstructObject<UDistributionVectorConstant>(this);
    static_cast<UDistributionVectorConstant*>(StartVelocity.Distribution)->Constant = ParticleModuleDefaults::Velocity::LinearVelocity;

    StartVelocityRadial.Distribution = FObjectFactory::ConstructObject<UDistributionFloatConstant>(this);
    static_cast<UDistributionFloatConstant*>(StartVelocityRadial.Distribution)->SetValue(ParticleModuleDefaults::Velocity::RadialVelocity);
}

void UParticleModuleVelocity::Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, float Interp, FBaseParticle* ParticleBase)
{
    if (!Owner) {
        UE_LOG(LogLevel::Warning, "[UParticleModuleVelocity::Spawn()] : There is no Owner!(FParticleEmitterInstance)");
        return;
    }


    // FIXME : 다른 곳에서 이미 EmitterOrigin으로 초기화하고 Location 모듈로 인해 초기화되고 있어서 여기서 EmitterOrigin 사용할 필요 있는지 확인해보기
    const FVector EmitterOrigin = Owner->GetEmitterOrigin();

    FVector LinearVelocity = StartVelocity.GetValue();
    float RadialSpeed = StartVelocityRadial.GetValue();

    FVector DirectionToCenter = ParticleBase->Location - EmitterOrigin;
    DirectionToCenter.Normalize();

    ParticleBase->Velocity = LinearVelocity + DirectionToCenter * RadialSpeed;
    ParticleBase->BaseVelocity = ParticleBase->Velocity;
}

EModuleType UParticleModuleVelocity::GetType() const
{
    return EModuleType::Spawn;
}
