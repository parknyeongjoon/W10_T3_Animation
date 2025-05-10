#include "AnimDataModel.h"
#include "Math/Transform.h"

void UAnimDataModel::GetBoneTrackNames(TArray<FName>& OutNames) const
{
    TArray<FName> TrackNames;
    for (const auto& Track : BoneAnimationTracks)
    {
        TrackNames.Add(Track.Name);
    }
    OutNames = TrackNames;
}

bool UAnimDataModel::IsValidBoneTrackName(const FName& TrackName) const
{
    for (const auto& Track : BoneAnimationTracks)
    {
        if (Track.Name == TrackName)
            return true;
    }

    return false;
}

const FBoneAnimationTrack& UAnimDataModel::GetBoneTrackByName(FName TrackName) const
{
    for (const auto& Track : BoneAnimationTracks)
    {
        if (Track.Name == TrackName)
        {
            return Track;
        }
    }

    return {};
}

FTransform UAnimDataModel::EvaluateBoneTrackTransform(FName TrackName, const FFrameTime& FrameTime, const EAnimInterpolationType& Interpolation) const
{
    return {};
}

FTransform UAnimDataModel::GetBoneTrackTransform(FName TrackName, const int32& FrameNumber) const
{
    
    const FBoneAnimationTrack* Track = nullptr;
    if (IsValidBoneTrackName(TrackName))
    {
        Track = &GetBoneTrackByName(TrackName);
    }

    if (Track)
    {
        const int32 KeyIndex = FrameNumber;
        if (Track->InternalTrackData.PosKeys.IsValidIndex(KeyIndex) &&
            Track->InternalTrackData.RotKeys.IsValidIndex(KeyIndex) &&
            Track->InternalTrackData.ScaleKeys.IsValidIndex(KeyIndex))
        {
            return {
                FQuat(Track->InternalTrackData.RotKeys[KeyIndex]),
                FVector(Track->InternalTrackData.PosKeys[KeyIndex]),
                FVector(Track->InternalTrackData.ScaleKeys[KeyIndex])
            };
        }
    }

    return FTransform::Identity;
}

void UAnimDataModel::GetBoneTrackTransforms(FName TrackName, const TArray<int32>& FrameNumbers, TArray<FTransform>& OutTransforms) const
{
    const FBoneAnimationTrack* Track = nullptr;
    if (IsValidBoneTrackName(TrackName))
    {
        Track = &GetBoneTrackByName(TrackName);
    }

    OutTransforms.SetNum(FrameNumbers.Num());

    if (Track)
    {
        for (int32 EntryIndex = 0; EntryIndex < FrameNumbers.Num(); ++EntryIndex)
        {
            OutTransforms[EntryIndex] = GetBoneTrackTransform(TrackName, FrameNumbers[EntryIndex]);
        }
    }
}

void UAnimDataModel::GetBoneTrackTransforms(FName TrackName, TArray<FTransform>& OutTransforms) const
{
    const FBoneAnimationTrack* Track = nullptr;
    if (IsValidBoneTrackName(TrackName))
    {
        Track = &GetBoneTrackByName(TrackName);
    }
	
    OutTransforms.SetNum(NumberOfKeys);

    if (Track)
    {
        for (int32 KeyIndex = 0; KeyIndex < NumberOfKeys; ++KeyIndex)
        {
            OutTransforms[KeyIndex] = GetBoneTrackTransform(TrackName, KeyIndex);
        }
    }
}

void UAnimDataModel::GetBoneTracksTransform(const TArray<FName>& TrackNames, const int32& FrameNumber, TArray<FTransform>& OutTransforms) const
{
    OutTransforms.SetNum(TrackNames.Num());
    for (int32 EntryIndex = 0; EntryIndex < TrackNames.Num(); ++EntryIndex)
    {
        OutTransforms[EntryIndex] = GetBoneTrackTransform(TrackNames[EntryIndex], FrameNumber);
    }
}
