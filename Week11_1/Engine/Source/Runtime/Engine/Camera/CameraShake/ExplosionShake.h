#pragma once
#include "CameraShakeBase.h"
class UExplosionShake : public UCameraShakeBase
{
    DECLARE_CLASS(UExplosionShake, UCameraShakeBase)
public:
    UExplosionShake();
    virtual ~UExplosionShake();
    virtual void MarkRemoveObject() override;
public:
    float MaxPitch = 5.f;
    float MaxYaw = 5.f;
    float MaxRoll = 2.f;
    float Frequency = 20.f;

    float LastPitch = 0.f;
    float LastYaw = 0.f;
    float LastRoll = 0.f;

    float AccumPitch = 0.f;
    float AccumYaw = 0.f;
    float AccumRoll = 0.f;

    virtual void UpdateShake(float DeltaTime, FVector& OutLoc, FRotator& OutRot, float& OutFOV) override;
};


