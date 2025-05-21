#include "ParticleModuleVelocity.h"
#include "Distributions//DistributionFloat.h"
#include "Distributions/DistributionVector.h"
#include "Particles/ParticleEmitterInstances.h"
UParticleModuleVelocity::UParticleModuleVelocity()
{
    StartVelocity.Distribution = new UDistributionVectorConstant();
    static_cast<UDistributionVectorConstant*>(StartVelocity.Distribution)->Constant = FVector(100.f, 0.f, 0.f);

    StartVelocityRadial.Distribution = new UDistributionFloatConstant();
    static_cast<UDistributionFloatConstant*>(StartVelocityRadial.Distribution)->SetValue(0.0f);
}

void UParticleModuleVelocity::Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, float Interp)
{
    if (!Owner) {
        UE_LOG(LogLevel::Warning, "[UParticleModuleVelocity::Spawn()] : There is no Owner!(FParticleEmitterInstance)");
        return;
    }

    const FVector EmitterOrigin = Owner->GetEmitterOrigin();

    for (int32 i = 0; i < Owner->ActiveParticles; ++i) {

        uint8* Address = Owner->ParticleData + i * Owner->ParticleStride;
        DECLARE_PARTICLE_PTR(Particle, Address);

        
        // FBaseParticle* Particle = Owner->GetParticle(i);
        if (!Particle) {
            continue;
        }

        FVector LinearVelocity = StartVelocity.GetValue();
        float RadialSpeed = StartVelocityRadial.GetValue();

        FVector DirectionToCenter = Particle->Location - EmitterOrigin;
        DirectionToCenter.Normalize();

        Particle->Velocity = LinearVelocity + DirectionToCenter * RadialSpeed;
        Particle->BaseVelocity = Particle->Velocity;
    }
}

void UParticleModuleVelocity::Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime)
{
    // 아무것도 하지 않음.
}
