#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

class USkeletalMeshComponent;
class UAnimSequenceBase;

class UAnimNotify : public UObject
{
    DECLARE_CLASS(UAnimNotify, UObject)
public:
    UAnimNotify() = default;
    virtual ~UAnimNotify() = default;

    virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation);
};

