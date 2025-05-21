#include "ParticleModuleColor.h"
#include <Particles/ParticleMacros.h>
#include "Runtime/Engine/Particles/ParticleEmitterInstances.h"
#include "ParticleModuleDefaults.h"

void UParticleModuleColor::InitializeDefaults()
{
    // FIXME : color alpha 구현
    /*UDistributionFloatUniform* DefaultDistributionFloat = FObjectFactory::ConstructObject<UDistributionFloatUniform>(nullptr);
    DefaultDistributionFloat->MinValue = 0.0f;
    DefaultDistributionFloat->MaxValue = 1.0f;
    StartAlpha.Distribution = DefaultDistributionFloat;
    for (int i = 0; i < 10; i++)
    {
        float t = StartAlpha.GetValue();
        t = t + 1;
    }*/

    // 초기값
    //StartColor.StartColor = FVector(1.0f, 1.0f, 1.0f);

    UDistributionVectorUniform* DefaultDistributionVector = FObjectFactory::ConstructObject<UDistributionVectorUniform>(nullptr);
    DefaultDistributionVector->MinValue = ParticleModuleDefaults::Color::MinColor;
    DefaultDistributionVector->MaxValue = ParticleModuleDefaults::Color::MaxColor;;
    StartColor.Distribution = DefaultDistributionVector;
}

void UParticleModuleColor::Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, float Interp, FBaseParticle* ParticleBase)
{
    FVector ColorVec = StartColor.GetValue();
    ParticleBase->Color = FColor(
        FMath::Clamp(int32(ColorVec.X * 255), 0, 255),
        FMath::Clamp(int32(ColorVec.Y * 255), 0, 255),
        FMath::Clamp(int32(ColorVec.Z * 255), 0, 255),
        255 // alpha
    );
}

void UParticleModuleColor::Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime)
{

}

EModuleType UParticleModuleColor::GetType() const
{
    return EModuleType::Spawn;
}
