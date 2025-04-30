#pragma once
#include "CameraShakeBase.h"

class UCurveFloat;

class UGunRecoilShake : public UCameraShakeBase
{
    DECLARE_CLASS(UGunRecoilShake, UCameraShakeBase)
public:
    UGunRecoilShake();
    virtual ~UGunRecoilShake() override;

    virtual void MarkRemoveObject() override;
public:
    UCurveFloat* PitchCurve = nullptr;
    UCurveFloat* YawCurve = nullptr;
    UCurveFloat* RollCurve = nullptr;

    float LastPitch = 0.f;
    float LastYaw = 0.f;
    float LastRoll = 0.f;

    virtual void UpdateShake(float DeltaTime, FVector& OutLoc, FRotator& OutRot, float& OutFOV) override;
};

