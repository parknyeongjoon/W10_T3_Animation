#pragma once
#include "UObject/Object.h"
#include "Math/Vector.h"
#include "Math/Rotator.h"
#include "UObject/ObjectMacros.h"

class FViewTarget;
class UCameraShakeBase;

struct FActiveCameraShakeInfo
{
    UCameraShakeBase* Instance = nullptr;

    bool IsFinished() const;
};

class UCameraShakeBase : public UObject
{
    DECLARE_CLASS(UCameraShakeBase, UObject)
public:
    UCameraShakeBase();
    virtual ~UCameraShakeBase();

    virtual void MarkRemoveObject();
public:
    float Duration = 0.2f;
    float ElapsedTime = 0.0f;
    float BlendInTime = 0.05f;
    float BlendOutTime = 0.05f;
    float Scale = 1.0f;

    bool bFinished = false;

    // 흔들림 계산 결과를 반환
    virtual void UpdateShake(float DeltaTime, FVector& OutLocShake, FRotator& OutRotShake, float& OutFOVShake);

    // 시간 누적 및 종료 체크
    virtual void Tick(float DeltaTime);

    // BlendIn / BlendOut 구간에서 가중치 반환
    float GetBlendWeight() const;
};
