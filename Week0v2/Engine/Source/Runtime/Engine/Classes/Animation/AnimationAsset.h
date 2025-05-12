#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

class UAnimationAsset : public UObject
{
    DECLARE_CLASS(UAnimationAsset, UObject)
public:
    UAnimationAsset() = default;
    UAnimationAsset(const UAnimationAsset&) = default;

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