#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "CoreUObject/UObject/ObjectFactory.h"

class UAnimationAsset : public UObject
{
    DECLARE_CLASS(UAnimationAsset, UObject)
public:
    UAnimationAsset() = default;

    virtual UObject* Duplicate(UObject* InOuter) override;
    virtual void DuplicateSubObjects(const UObject* Source, UObject* InOuter) override;
    virtual void PostDuplicate() override;

};

struct FAnimExtractContext
{

    double CurrentTime;
    bool bLooping;
    bool bExtractRootMotion;

    FAnimExtractContext(float InCurrentTime, bool bInLooping = true, bool bInExtractRootMotion = false)
        : CurrentTime(InCurrentTime), bLooping(bInLooping), bExtractRootMotion(bInExtractRootMotion)
    {
    }
};