#pragma once
#include "Animation/AnimTypes.h"
#include "Container/Array.h"
#include "HAL/PlatformType.h"

class FTransform;
struct FFrameRate;
struct FFrameTime;

class IAnimationDataModel
{
public:
    virtual ~IAnimationDataModel() = default;
    
    virtual double GetPlayLength() const = 0; // Total length of play-able animation data
    virtual int32 GetNumberOfFrames() const = 0; // Total number of frames of animation data stored
    virtual int32 GetNumberOfKeys() const = 0; // Total number of animation data keys stored
    virtual int32 GetNumBoneTracks() const = 0;
    virtual int32 GetNumberOfCurves() const = 0;
    virtual FFrameRate GetFrameRate() const = 0; // Frame rate at which the animation data is key-ed
    
    virtual const TArray<FBoneAnimationTrack>& GetBoneAnimationTracks() const = 0;
    virtual void GetBoneTrackNames(TArray<FName>& OutNames) const = 0;
    virtual bool IsValidBoneTrackName(const FName& TrackName) const = 0;
    virtual const FBoneAnimationTrack& GetBoneTrackByName(FName TrackName) const = 0;

    virtual FTransform EvaluateBoneTrackTransform(FName TrackName, const FFrameTime& FrameTime, const EAnimInterpolationType& Interpolation) const = 0;
    virtual FTransform GetBoneTrackTransform(FName TrackName, const int32& FrameNumber) const = 0;
    virtual void GetBoneTrackTransforms(FName TrackName, const TArray<int32>& FrameNumbers, TArray<FTransform>& OutTransforms) const = 0;
    virtual void GetBoneTrackTransforms(FName TrackName, TArray<FTransform>& OutTransforms) const = 0;
    virtual void GetBoneTracksTransform(const TArray<FName>& TrackNames, const int32& FrameNumber, TArray<FTransform>& OutTransforms) const = 0;

    virtual FAnimationCurveData GetCurve() const = 0;
};
