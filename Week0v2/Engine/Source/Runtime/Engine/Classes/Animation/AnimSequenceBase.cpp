#include "AnimSequenceBase.h"
#include "AnimNodeBase.h"
#include "AnimData/AnimDataModel.h"

void UAnimSequenceBase::SetData(const FString& FilePath)
{
    SetData(TestFBXLoader::GetAnimData(FilePath));
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
    //float TimeToExtract = ExtractionContext.CurrentTime;

    //// 루프가 설정된 경우, 시간이 애니메이션 길이를 넘어가면 다시 0으로 감싼다.
    //if (ExtractionContext.bLooping && TimeToExtract >= DataModel->PlayLength)
    //{
    //    TimeToExtract = FMath::Fmod(TimeToExtract, DataModel->PlayLength);
    //}

    //// 각 Bone의 Transform을 업데이트
    //for (int32 BoneIndex = 0; BoneIndex < OutPose.Pose.BoneTransforms.Num(); ++BoneIndex)
    //{
    //    // 현재 시간에서 본(Bone) 트랜스폼 추출
    //    FTransform BoneTransform = DataModel->GetBoneTrackTransform(BoneIndex, TimeToExtract);
    //    .Pose.SetBoneTransform(BoneIndex, BoneTransform);
    //}

    //// Root Motion 처리
    //if (ExtractionContext.bExtractRootMotion)
    //{
    //    OutPose.Pose.RootMotionTransform = BoneTracks[0].EvaluateTransformAtTime(TimeToExtract);
    //}
}
