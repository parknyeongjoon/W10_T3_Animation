#pragma once
#include "CameraShakeBase.h"
class UDashShake :
    public UCameraShakeBase
{
    DECLARE_CLASS(UDashShake, UCameraShakeBase)
public:
    UDashShake();
    virtual ~UDashShake();
public:
    float ForwardOffset = 15.f;  // 앞으로 튀는 거리
    float RollAmount = 3.f;
    float Frequency = 20.f;

    float LastOffset = 0.f;
    float LastRoll = 0.f;

    virtual void UpdateShake(float DeltaTime, FVector& OutLoc, FRotator& OutRot, float& OutFOV) override;

};

