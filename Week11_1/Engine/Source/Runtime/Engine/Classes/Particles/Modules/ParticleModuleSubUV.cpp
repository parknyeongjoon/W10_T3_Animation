#include "ParticleModuleSubUV.h"
#include "Engine/Particles/ParticleEmitterInstances.h"
#include "ParticleModuleRequired.h"
#include <Particles/ParticleMacros.h>

void UParticleModuleSubUV::Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime)
{
    const int32 TotalFrames = Owner->RequiredModule->GetTotalSubImages();
    if (TotalFrames <= 1) return;

    BEGIN_UPDATE_LOOP;
    {
        float t = Particle.RelativeTime;
        float FrameIndex = t * TotalFrames;

        Particle.SubImageIndex = FrameIndex;
        UE_LOG(LogLevel::Display, "SubImageIndex : %f", Particle.SubImageIndex);
    }
    END_UPDATE_LOOP;
}

EModuleType UParticleModuleSubUV::GetType() const
{
    return EModuleType::Update;
}
