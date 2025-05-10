#pragma once
#include "AnimationAsset.h"
#include "AnimTypes.h"
class UAnimSequenceBase :
    public UAnimationAsset
{
    TArray<FAnimNotifyEvent> Notifies;
};

