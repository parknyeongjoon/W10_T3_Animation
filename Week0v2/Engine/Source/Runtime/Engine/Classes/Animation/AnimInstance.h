#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "AnimTypes.h"
class USkeletalMeshComponent;

class UAnimInstance : public UObject
{
    DECLARE_CLASS(UAnimInstance, UObject)
public:
    UAnimInstance() = default;
    USkeletalMeshComponent* GetSkelMeshComponent() const;
    virtual void TriggerAnimNotifies(float DeltaSeconds);
    virtual void NativeUpdateAnimation(float DeltaSeconds);
};

