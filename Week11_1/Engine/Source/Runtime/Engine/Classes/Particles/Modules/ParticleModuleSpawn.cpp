#include "ParticleModuleSpawn.h"
#include "Runtime/Engine/Particles/ParticleEmitterInstances.h"
#include "Engine/Particles/ParticleHelper.h"
#include "Particles/Modules/ParticleModuleDefaults.h"
UParticleModuleSpawn::UParticleModuleSpawn()
{
    InitializeDefaults();

    //BurstList.Add({ 0.2f, 20 });
    //BurstList.Add({ 0.8f, 50 });
}

void UParticleModuleSpawn::InitializeDefaults()
{
    // Rate = 10.0 고정
    UDistributionFloatConstant* ConstantRate = FObjectFactory::ConstructObject<UDistributionFloatConstant>(nullptr);
    ConstantRate->SetValue(ParticleModuleDefaults::Spawn::Rate); // 원하는 생성 속도
    Rate.Distribution = ConstantRate;

    // RateScale = 1.0 고정
    UDistributionFloatConstant* ConstantScale = FObjectFactory::ConstructObject<UDistributionFloatConstant>(nullptr);
    ConstantScale->SetValue(ParticleModuleDefaults::Spawn::RateScale); // 원하는 스케일 값
    RateScale.Distribution = ConstantScale;  
}

int32 UParticleModuleSpawn::ComputeSpawnCount(float DeltaTime)
{

    // Rate와 RateScale의 실제 값을 가져옴
    float RateValue = Rate.GetValue();
    float ScaleValue = RateScale.GetValue();

    float EffectiveRate = RateValue * ScaleValue;

    // 실제 생성해야 하는 파티클 수 = (초당 Rate) * 시간
    float ParticlesToSpawn = EffectiveRate * DeltaTime + SpawnRemainder;

    int32 IntegerPart = static_cast<int32>(ParticlesToSpawn);
    
    SpawnRemainder = ParticlesToSpawn - IntegerPart;

    return IntegerPart;
}


EModuleType UParticleModuleSpawn::GetType() const
{
    // FIXME : 왜 Spawn 함수가 없는데 None으로 지정하면 안되지?
    return EModuleType::Spawn;
}

void UParticleModuleSpawn::Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime)
{
    //CheckBurst(Owner, DeltaTime);
}

void UParticleModuleSpawn::CheckBurst(FParticleEmitterInstance* Owner, float DeltaTime)
{
    float PrevTime = Owner->EmitterTime - DeltaTime;
    float CurrTime = Owner->EmitterTime;

    for (FBurstInfo& Burst : BurstList)
    {
        if (!Burst.bFired && Burst.Time >= PrevTime && Burst.Time < CurrTime)
        {
            FVector SpawnLoc = Owner->GetEmitterOrigin();
            FVector SpawnVel = FVector::ZeroVector;
            Owner->SpawnParticles(Burst.Count, Burst.Time, 0.0f, SpawnLoc, SpawnVel);

            Burst.bFired = true;
        }
    }
}

void UParticleModuleSpawn::ResetBurst()
{
    for (FBurstInfo& Burst : BurstList) {
        Burst.bFired = false;
    }
}

 