#pragma once
#include "IAnimationDataModel.h"
#include "UObject/Object.h"
#include "Animation/AnimTypes.h"
#include "UObject/ObjectMacros.h"
#include "Misc/Frame/FrameRate.h"

class UAnimDataModel : public UObject, IAnimationDataModel
{
    DECLARE_CLASS(UAnimDataModel, UObject)
public:
    UAnimDataModel() = default;
    
    TArray<FBoneAnimationTrack> BoneAnimationTracks;
    float PlayLength;
    FFrameRate FrameRate;
    int32 NumberOfFrames;
    int32 NumberOfKeys;
    FAnimationCurveData CurveData;
    // Notifier?

    double GetPlayLength() const override { return FrameRate.AsSeconds(NumberOfFrames); } // Total length of play-able animation data
    int32 GetNumberOfFrames() const override { return NumberOfFrames; } // Total number of frames of animation data stored
    int32 GetNumberOfKeys() const override { return NumberOfKeys; } // Total number of animation data keys stored
    int32 GetNumBoneTracks() const override { return BoneAnimationTracks.Num(); }
    int32 GetNumberOfCurves() const override { return CurveData.Curves.Num(); }
    FFrameRate GetFrameRate() const override { return FrameRate; } // Frame rate at which the animation data is key-ed
    
    const TArray<FBoneAnimationTrack>& GetBoneAnimationTracks() const override { return BoneAnimationTracks; }
    void GetBoneTrackNames(TArray<FName>& OutNames) const override;
    bool IsValidBoneTrackName(const FName& TrackName) const override;
    const FBoneAnimationTrack& GetBoneTrackByName(FName TrackName) const override;

    FTransform EvaluateBoneTrackTransform(FName TrackName, const FFrameTime& FrameTime, const EAnimInterpolationType& Interpolation) const override;
    FTransform GetBoneTrackTransform(FName TrackName, const int32& FrameNumber) const override;
    void GetBoneTrackTransforms(FName TrackName, const TArray<int32>& FrameNumbers, TArray<FTransform>& OutTransforms) const override;
    void GetBoneTrackTransforms(FName TrackName, TArray<FTransform>& OutTransforms) const override;
    void GetBoneTracksTransform(const TArray<FName>& TrackNames, const int32& FrameNumber, TArray<FTransform>& OutTransforms) const override;

    FAnimationCurveData GetCurve() const override { return CurveData; }
};