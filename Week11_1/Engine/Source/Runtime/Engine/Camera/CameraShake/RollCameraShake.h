#pragma once
#include "CameraShakeBase.h"
class URollCameraShake :
    public UCameraShakeBase
{
    DECLARE_CLASS(URollCameraShake,UCameraShakeBase)
public:
    URollCameraShake();
    virtual ~URollCameraShake();
public:
    float TotalRotation = -360.f;  // 한 바퀴
    float LastPitch = 0.f;

    virtual void UpdateShake(float DeltaTime, FVector& OutLoc, FRotator& OutRot, float& OutFOV) override;

};

