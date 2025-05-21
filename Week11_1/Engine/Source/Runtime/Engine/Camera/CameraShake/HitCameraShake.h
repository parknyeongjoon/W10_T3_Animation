#pragma once
#include "CameraShakeBase.h"
class UHitCameraShake :
    public UCameraShakeBase
{
    DECLARE_CLASS(UHitCameraShake, UCameraShakeBase)
public:
    UHitCameraShake();
    virtual ~UHitCameraShake();

    float RotationScale = 15.f;  // 전체 회전 강도
    float LastPitch = 0.f;
    float LastYaw = 0.f;

    FVector ImpactDirection = FVector::ZeroVector;  // 피격 방향 (World 기준)
    FVector CameraLocation = FVector::ZeroVector;   // 시작 시점 카메라 위치
    FRotator CameraRotation = FRotator::ZeroRotator;

    virtual void UpdateShake(float DeltaTime, FVector& OutLoc, FRotator& OutRot, float& OutFOV) override;

};

