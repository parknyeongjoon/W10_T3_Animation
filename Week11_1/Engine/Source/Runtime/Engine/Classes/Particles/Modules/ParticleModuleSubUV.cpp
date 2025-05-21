#include "ParticleModuleSubUV.h"
#include "Engine/Particles/ParticleEmitterInstances.h"
#include "ParticleModuleRequired.h"
#include <Particles/ParticleMacros.h>
#include "cmath"
void UParticleModuleSubUV::Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime)
{
    const int32 TotalFrames = Owner->RequiredModule->GetTotalSubImages();
    if (TotalFrames <= 1) return;

    BEGIN_UPDATE_LOOP;
    {
        // SubUVPayLoad 위치 = 파티클 시작 주소 + 모듈 오프셋
        FFullSubUVPayload* SubUVPayload = (FFullSubUVPayload*)(Address + GetPayloadOffset());

        // 파티클 상대 시간 비율 계산
        float Age = Particle.RelativeTime;
        float Lifetime = Particle.Lifetime;

        // 시간 경과 비율 (0 ~ 1)
        float t = Lifetime > 0.f ? Age / Lifetime : 0.f;
        //t = FMath::Clamp(t, 0.f, 1.f);
        float AnimProgress = SubUVSpeed * t;
        float FrameIndex = AnimProgress * TotalFrames;

        while (FrameIndex >= TotalFrames)
        {
            FrameIndex -= TotalFrames;
        }
        //float FrameIndex = FinalOutVal * t * (TotalFrames - 1);  // 마지막 프레임까지
        SubUVPayload->ImageIndex = FrameIndex;
        //UE_LOG(LogLevel::Display, "SubUVIndex : %f", FrameIndex);
    }
    END_UPDATE_LOOP;
}

void UParticleModuleSubUV::Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, float Interp, FBaseParticle* ParticleBase)
{
    FFullSubUVPayload* SubUVPayload = (FFullSubUVPayload*)(((uint8*)ParticleBase) + GetPayloadOffset());
    SubUVPayload->ImageIndex = 0.0f;
    SubUVPayload->RandomImageTime = 0.0f;
}

EModuleType UParticleModuleSubUV::GetType() const
{
    return EModuleType::Both;
}

int32 UParticleModuleSubUV::GetPayloadSize() const
{
    return sizeof(FFullSubUVPayload);
}
