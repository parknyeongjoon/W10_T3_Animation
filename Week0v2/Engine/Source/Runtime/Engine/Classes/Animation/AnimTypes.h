#pragma once
#include "Define.h"
struct FAnimNotifyEvent
{
    float TriggerTime;
    float Duration;
    FName NotifyName;
};

struct FRawAnimSequenceTrack
{
    TArray<FVector> PosKeys;
    TArray<FQuat> RotKeys;
    TArray<FVector> ScaleKeys;
};

struct FBoneAnimationTrack
{
    FName Name;
    FRawAnimSequenceTrack InternalTrack;
};