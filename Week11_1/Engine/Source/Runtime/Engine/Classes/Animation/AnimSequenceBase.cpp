#include "AnimSequenceBase.h"
#include "AnimNodeBase.h"
#include "AnimData/AnimDataModel.h"
#include "Delegates/FFunctor.h"
#include "UObject/Casts.h"

UAnimSequenceBase::UAnimSequenceBase()
{
    AddNotifyTrack("0");
}

UAnimSequenceBase::UAnimSequenceBase(const UAnimSequenceBase& Other)
    :UAnimationAsset(Other),
    Notifies(Other.Notifies),
    RateScale(Other.RateScale),
    DataModel(Other.DataModel)
{
}

UObject* UAnimSequenceBase::Duplicate(UObject* InOuter)
{
    UAnimSequenceBase* NewComp = FObjectFactory::ConstructObjectFrom<UAnimSequenceBase>(this, InOuter);
    NewComp->DuplicateSubObjects(this, InOuter);
    NewComp->PostDuplicate();
    return NewComp;
}

void UAnimSequenceBase::DuplicateSubObjects(const UObject* Source, UObject* InOuter)
{
    Super::DuplicateSubObjects(Source, InOuter);
    UAnimSequenceBase* original = Cast<UAnimSequenceBase>(Source);
    for (int i = 0; i < Notifies.Num(); ++i)
    {
        Notifies[i] = original->Notifies[i];
    }
    DataModel = original->DataModel;
}

void UAnimSequenceBase::PostDuplicate()
{
}

void UAnimSequenceBase::SetData(const FString& FilePath)
{
    SetData(FFBXLoader::GetAnimData(FilePath));
}

void UAnimSequenceBase::AddNotify(int32 TargetTrackIndex, float Second, TDelegate<void()> OnNotify, float Duration, const FName& NotifyName)
{
    if (!AnimNotifyTracks.IsValidIndex(TargetTrackIndex))
    {
        return;
    }
    if (NotifyName.ToString().IsEmpty())
    {
        return;
    }

    const int32 Index = Notifies.Add(FAnimNotifyEvent());
    FAnimNotifyEvent& NotifyEvent = Notifies[Index];
    NotifyEvent.TrackIndex = TargetTrackIndex;
    NotifyEvent.OnNotify = OnNotify;
    NotifyEvent.TriggerTime = Second;
    NotifyEvent.Duration = Duration;
    NotifyEvent.NotifyName = NotifyName;
    SortNotifies();

    AnimNotifyTracks[TargetTrackIndex].NotifyIndices.Add(Index);
}

void UAnimSequenceBase::AddNotify(int32 TargetTrackIndex, float Second, std::function<void()> OnNotify, float Duration, const FName& NotifyName)
{
    if (!AnimNotifyTracks.IsValidIndex(TargetTrackIndex))
    {
        return;
    }
    if (NotifyName.ToString().IsEmpty())
    {
        return;
    }

    const int32 Index = Notifies.Add(FAnimNotifyEvent());
    FAnimNotifyEvent& NotifyEvent = Notifies[Index];
    NotifyEvent.TrackIndex = TargetTrackIndex;
    NotifyEvent.OnNotify.BindLambda(OnNotify);
    NotifyEvent.TriggerTime = Second;
    NotifyEvent.Duration = Duration;
    NotifyEvent.NotifyName = NotifyName;
    SortNotifies();

    AnimNotifyTracks[TargetTrackIndex].NotifyIndices.Add(Index);
}

void UAnimSequenceBase::UpdateNotify(const int32 NotifyIndexToUpdate, const float NewTriggerTime, const float NewDuration, const int32 NewTrackIndex, const FName& NewNotifyName)
{
    if (!Notifies.IsValidIndex(NotifyIndexToUpdate))
    {
        return;
    }
    if (!AnimNotifyTracks.IsValidIndex(NewTrackIndex))
    {
        return;
    }

    FAnimNotifyEvent& Notify = Notifies[NotifyIndexToUpdate];
    const int32 OldTrackIndex = Notify.TrackIndex;

    Notify.TriggerTime = NewTriggerTime;
    Notify.Duration = NewDuration;

    if (OldTrackIndex != NewTrackIndex)
    {
        if (AnimNotifyTracks.IsValidIndex(OldTrackIndex))
        {
            AnimNotifyTracks[OldTrackIndex].NotifyIndices.RemoveSingle(NotifyIndexToUpdate);
        }
        AnimNotifyTracks[NewTrackIndex].NotifyIndices.Add(NotifyIndexToUpdate);
        Notify.TrackIndex = NewTrackIndex;
    }

    if (!NewNotifyName.ToString().IsEmpty())
    {
        Notify.NotifyName = NewNotifyName;
    }
}

void UAnimSequenceBase::SortNotifies()
{
    Notifies.Sort();
}

bool UAnimSequenceBase::RemoveNotifyEvent(int32 NotifyIndexToRemove)
{
    if (!Notifies.IsValidIndex(NotifyIndexToRemove))
    {
        return false;
    }

    const FAnimNotifyEvent& EventToRemove = Notifies[NotifyIndexToRemove];
    if (AnimNotifyTracks.IsValidIndex(EventToRemove.TrackIndex))
    {
        // Remove the global notify index from its track's list
        AnimNotifyTracks[EventToRemove.TrackIndex].NotifyIndices.RemoveSingle(NotifyIndexToRemove);
    }

    Notifies.RemoveAt(NotifyIndexToRemove);

    // Adjust NotifyIndices in all tracks for global indices that shifted
    for (FAnimNotifyTrack& Track : AnimNotifyTracks)
    {
        for (int32 i = Track.NotifyIndices.Num() - 1; i >= 0; --i)
        {
            if (Track.NotifyIndices[i] > NotifyIndexToRemove)
            {
                Track.NotifyIndices[i]--;
            }
            // If an index became invalid somehow (e.g. points to the removed one, though RemoveAt handles shifting), clean up.
            // This check might be overly cautious if RemoveAt and the loop above are correct.
            else if (Track.NotifyIndices[i] == NotifyIndexToRemove)
            {
                Track.NotifyIndices.RemoveAt(i); // Should have been caught by RemoveSingle if it was the one
            }
        }
    }
    return true;
}

bool UAnimSequenceBase::RemoveNotifies(const TArray<FName>& NotifiesToRemove)
{
    // @todo Track에서도 제거할 것
    bool bSequenceModified = false;
    for (int32 NotifyIndex = Notifies.Num() - 1; NotifyIndex >= 0; --NotifyIndex)
    {
        FAnimNotifyEvent& AnimNotify = Notifies[NotifyIndex];
        if (NotifiesToRemove.Contains(AnimNotify.NotifyName))
        {
            Notifies.RemoveAt(NotifyIndex);
            bSequenceModified = true;
        }
    }
    
    return bSequenceModified;
}

void UAnimSequenceBase::RemoveNotifies()
{
    if (Notifies.Num() == 0)
    {
        return;
    }

    Notifies.Empty();
    AnimNotifyTracks.Empty();
}

void UAnimSequenceBase::RenameNotifies(FName InOldName, FName InNewName)
{
    for(FAnimNotifyEvent& Notify : Notifies)
    {
        if(Notify.NotifyName == InOldName)
        {
            Notify.NotifyName = InNewName;
        }
    }
}

void UAnimSequenceBase::ResetNotifies()
{
    for (FAnimNotifyEvent& Notify : Notifies)
    {
        Notify.bIsTriggered = false;
    }
}

void UAnimSequenceBase::AddNotifyTrack(const FName& NotifyTrackName)
{
    if (NotifyTrackName.ToString().IsEmpty())
    {
        return;
    }

    const int32 Index = AnimNotifyTracks.Add(FAnimNotifyTrack());
    FAnimNotifyTrack& NotifyTrack = AnimNotifyTracks[Index];
    NotifyTrack.TrackName = NotifyTrackName;
}

void UAnimSequenceBase::RenameNotifyTrack(int32 TrackIndex, const FName& NewTrackName)
{
    if (!AnimNotifyTracks.IsValidIndex(TrackIndex))
    {
        return;
    }
    if (NewTrackName.ToString().IsEmpty())
    {
        return;
    }
    int32 ExistingTrackIndex = FindNotifyTrackIndexByName(NewTrackName);
    if (ExistingTrackIndex != INDEX_NONE && ExistingTrackIndex != TrackIndex)
    {
        return;
    }
    AnimNotifyTracks[TrackIndex].TrackName = NewTrackName;
}

int32 UAnimSequenceBase::FindNotifyTrackIndexByName(const FName& NotifyTrackName)
{
    for (int32 Index = 0; Index < AnimNotifyTracks.Num(); ++Index)
    {
        if (AnimNotifyTracks[Index].TrackName == NotifyTrackName)
        {
            return Index;
        }
    }

    return INDEX_NONE;
}

void UAnimSequenceBase::GetAnimationPose(FPoseContext& OutPose, const FAnimExtractContext& ExtractionContext) const
{
    float TimeToExtract = ExtractionContext.CurrentTime;

    // 루프가 설정된 경우, 시간이 애니메이션 길이를 넘어가면 다시 0으로 감싼다.
    if (ExtractionContext.bLooping && TimeToExtract >= DataModel->PlayLength)
    {
        TimeToExtract = FMath::Fmod(TimeToExtract, DataModel->PlayLength);
    }

    TArray<FName> BoneNames;
    DataModel->GetBoneTrackNames(BoneNames);

    float FrameRate = DataModel->FrameRate.AsDecimal(); // 프레임 레이트를 float로 변환
    float FramePosition = TimeToExtract * FrameRate;

    int32 FrameIndex = static_cast<int32>(FramePosition); // 정수 인덱스
    float Fraction = FramePosition - FrameIndex; // 보간 값

    for (const auto& Name : BoneNames)
    {
        FTransform Transform;
        const FBoneAnimationTrack* Track = nullptr;

        if (DataModel->IsValidBoneTrackName(Name))
        {
            Track = &DataModel->GetBoneTrackByName(Name);
        }

        if (Track)
        {
            // 보간 처리
            if (Track->InternalTrackData.RotKeys.IsValidIndex(FrameIndex) &&
                Track->InternalTrackData.RotKeys.IsValidIndex(FrameIndex + 1))
            {
                FQuat RotA = Track->InternalTrackData.RotKeys[FrameIndex];
                FQuat RotB = Track->InternalTrackData.RotKeys[FrameIndex + 1];
                Transform.SetRotation(FQuat::Slerp(RotA, RotB, Fraction));
            }

            if (Track->InternalTrackData.PosKeys.IsValidIndex(FrameIndex) &&
                Track->InternalTrackData.PosKeys.IsValidIndex(FrameIndex + 1))
            {
                FVector PosA = Track->InternalTrackData.PosKeys[FrameIndex];
                FVector PosB = Track->InternalTrackData.PosKeys[FrameIndex + 1];
                Transform.SetLocation(FMath::Lerp(PosA, PosB, Fraction));
            }

            if (Track->InternalTrackData.ScaleKeys.IsValidIndex(FrameIndex) &&
                Track->InternalTrackData.ScaleKeys.IsValidIndex(FrameIndex + 1))
            {
                FVector ScaleA = Track->InternalTrackData.ScaleKeys[FrameIndex];
                FVector ScaleB = Track->InternalTrackData.ScaleKeys[FrameIndex + 1];
                Transform.SetScale(FMath::Lerp(ScaleA, ScaleB, Fraction));
            }
        }
        OutPose.Pose.BoneTransforms.Add(Transform);
    }
}

void UAnimSequenceBase::EvaluateCurveData(FBlendedCurve& OutCurve, const FAnimExtractContext& ExtractionContext) const
{
}
