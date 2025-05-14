#include "AnimSequenceBase.h"
#include "AnimNodeBase.h"
#include "AnimData/AnimDataModel.h"
#include "UObject/Casts.h"

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

void UAnimSequenceBase::SortNotifies()
{
    Notifies.Sort();
}

bool UAnimSequenceBase::RemoveNotifies(const TArray<FName>& NotifiesToRemove)
{
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
