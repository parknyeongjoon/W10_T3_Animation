#pragma once
#include "UObject/Object.h"
#include "Animation/AnimTypes.h"
class UAnimDataModel :
    public UObject
{
    TArray<FBoneAnimationTrack> BoneAnimationTracks;
    float PlayLength;
    //FFrameRate FrameRate;
    int32 NumberOfFrames;
    int32 NumberOfKeys;
    //FAnimationCurveData CurveData;

    //virtual const TArray<FBoneAnimationTrack>& GetBoneAnimationTracks() const override;
};

