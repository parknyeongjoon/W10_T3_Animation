#pragma once
#include "CameraShakeBase.h"
class UGunRecoilShake : public UCameraShakeBase
{
public:
    float TotalRecoil = 5.f; // 튀는 각도
    float Duration = 0.2f;
    float Elapsed = 0.f;

    virtual void UpdateShake(float DeltaTime, FVector& OutLoc, FRotator& OutRot, float& OutFOV) override;
};

