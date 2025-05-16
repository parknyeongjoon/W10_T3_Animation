#include "AnimDataModel.h"
#include "Math/Transform.h"
#include "Serialization/Archive.h"

UAnimDataModel::UAnimDataModel(const FName& FilePath)
{
    
}

UAnimDataModel::UAnimDataModel(const TArray<FBoneAnimationTrack>& InAnimation)
{
    BoneAnimationTracks = InAnimation;
}


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
    const float Alpha = Interpolation == EAnimInterpolationType::Step ? FMath::RoundToFloat(FrameTime.GetSubFrame()) : FrameTime.GetSubFrame();

    if (FMath::IsNearlyEqual(Alpha, 1.0f))
    {
        return GetBoneTrackTransform(TrackName, FrameTime.CeilToFrame());
    }
    else if (FMath::IsNearlyZero(Alpha))
    {
        return GetBoneTrackTransform(TrackName, FrameTime.FloorToFrame());
    }
	
    const FTransform From = GetBoneTrackTransform(TrackName, FrameTime.FloorToFrame());
    const FTransform To = GetBoneTrackTransform(TrackName, FrameTime.CeilToFrame());

    FTransform Blend = FTransform::Blend(From, To, Alpha);
    return Blend;
}

FTransform UAnimDataModel::GetBoneTrackTransform(FName TrackName, const int32& FrameNumber) const
{
    FTransform Result;
    
    const FBoneAnimationTrack* Track = nullptr;
    if (IsValidBoneTrackName(TrackName))
    {
        Track = &GetBoneTrackByName(TrackName);
    }

    if (Track)
    {
        const int32 KeyIndex = FrameNumber;
        if (Track->InternalTrackData.RotKeys.IsValidIndex(KeyIndex))
        {
            Result.SetRotation(Track->InternalTrackData.RotKeys[KeyIndex]);
        }
        if (Track->InternalTrackData.PosKeys.IsValidIndex(KeyIndex))
        {
            Result.SetLocation(Track->InternalTrackData.PosKeys[KeyIndex]);
        }
        if (Track->InternalTrackData.ScaleKeys.IsValidIndex(KeyIndex))
        {
            Result.SetScale(Track->InternalTrackData.ScaleKeys[KeyIndex]);
        }
    }

    return Result;
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

void UAnimDataModel::Serialize(FArchive& Ar) const
{
    Ar << Name << BoneAnimationTracks << PlayLength << FrameRate << NumberOfFrames << NumberOfKeys << CurveData;
}

void UAnimDataModel::Deserialize(FArchive& Ar)
{
    Ar >> Name >> BoneAnimationTracks >> PlayLength >> FrameRate >> NumberOfFrames >> NumberOfKeys >> CurveData;
}
