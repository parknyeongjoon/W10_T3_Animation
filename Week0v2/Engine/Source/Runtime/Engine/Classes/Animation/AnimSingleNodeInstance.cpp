#include "AnimSingleNodeInstance.h"
#include "Components/PrimitiveComponents/MeshComponents/SkeletalMeshComponent.h"
#include "Animation/AnimSequence.h"
#include "Animation/AnimData/AnimDataModel.h"
#include <Math/JungleMath.h>

UAnimSingleNodeInstance::UAnimSingleNodeInstance()
    :CurrentAsset(nullptr),
    ElapsedTime(0.f),
    PlayRate(1.f),
    bLooping(true),
    bPlaying(false),
    bReverse(false),
    PreviousTime(0.f),
    LoopStartFrame(0),
    LoopEndFrame(0),
    CurrentKey(0)
{
    CurrentAsset = FObjectFactory::ConstructObject<UAnimSequence>(this);
}

UAnimSingleNodeInstance::UAnimSingleNodeInstance(const UAnimSingleNodeInstance& Other)
    :UAnimInstance(Other),
    CurrentAsset(nullptr),
    ElapsedTime(Other.ElapsedTime),
    PreviousTime(Other.PreviousTime),
    PlayRate(Other.PlayRate),
    bLooping(Other.bLooping),
    bPlaying(Other.bPlaying),
    bReverse(Other.bReverse),
    LoopStartFrame(Other.LoopStartFrame),
    LoopEndFrame(Other.LoopEndFrame),
    CurrentKey(Other.CurrentKey)
{
}

UObject* UAnimSingleNodeInstance::Duplicate(UObject* InOuter)
{
    UAnimSingleNodeInstance* NewComp = FObjectFactory::ConstructObjectFrom<UAnimSingleNodeInstance>(this, InOuter);
    NewComp->DuplicateSubObjects(this, InOuter);
    NewComp->PostDuplicate();
    return NewComp;
}

void UAnimSingleNodeInstance::SetAnimationAsset(UAnimSequence* NewAsset, bool bIsLooping, float InPlayRate)
{
    if (NewAsset != CurrentAsset)
    {
        CurrentAsset = NewAsset;
    }

    USkeletalMeshComponent* MeshComponent = GetOwningComponent();
    if (MeshComponent)
    {
        if (MeshComponent->GetSkeletalMesh() == nullptr)
        {
            CurrentAsset = nullptr;
        }
    }

    bLooping = bIsLooping;
    PlayRate = InPlayRate;
    ElapsedTime = 0.f;
}

void UAnimSingleNodeInstance::NativeInitializeAnimation()
{
}

void UAnimSingleNodeInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    UAnimInstance::NativeUpdateAnimation(DeltaSeconds);

    USkeletalMeshComponent* SkeletalMeshComp = GetOwningComponent();
    USkeletalMesh* SkeletalMesh = SkeletalMeshComp->GetSkeletalMesh();
    if (!CurrentAsset ||!SkeletalMesh||!SkeletalMesh->GetSkeleton()) return;

    const UAnimDataModel* DataModel = CurrentAsset->GetDataModel();
    const int32 FrameRate = DataModel->GetFrameRate().Numerator;    // Number of Frames per second);    // Number of Frames per second
    const int32 NumFrames = DataModel->GetNumberOfFrames();

    LoopStartFrame = FMath::Clamp(LoopStartFrame, 0, NumFrames - 2);
    LoopEndFrame = FMath::Clamp(LoopEndFrame, LoopStartFrame + 1, NumFrames - 1);
    const float StartTime = static_cast<float>(LoopStartFrame) / static_cast<float>(FrameRate);
    const float EndTime = static_cast<float>(LoopEndFrame) / static_cast<float>(FrameRate);

    if (bPlaying)
    {
        float DeltaPlayTime = DeltaSeconds * PlayRate;

        if (!bLooping)
        {
            if (!bReverse&&ElapsedTime == EndTime)
            {
                ElapsedTime = StartTime;
            }
            else if (bReverse && ElapsedTime == StartTime)
            {
                ElapsedTime = EndTime;
            }   
        }

        PreviousTime = ElapsedTime;
        ElapsedTime += DeltaPlayTime;
        CurrentKey = static_cast<int32>(ElapsedTime * FrameRate);
        //CurrentAsset->EvaluateAnimNotify()


        if (bLooping)
        {
            if (ElapsedTime > EndTime)
            {
                ElapsedTime = StartTime + FMath::Fmod(ElapsedTime - StartTime, EndTime - StartTime);
            }
            else if (ElapsedTime <= StartTime)
            {
                ElapsedTime = EndTime - FMath::Fmod(EndTime - ElapsedTime, EndTime - StartTime);
            }
        }
        else
        {
            if (!bReverse && ElapsedTime >= EndTime)
            {
                ElapsedTime = EndTime;
                bPlaying = false;
            }
            else if (bReverse && ElapsedTime <= StartTime)
            {
                ElapsedTime = StartTime;
                bPlaying = false;
            }
        }
    }
    else
    {
        PreviousTime = ElapsedTime;
        ElapsedTime = static_cast<float>(CurrentKey) / static_cast<float>(FrameRate);
    }

    FPoseContext Pose;
    FAnimExtractContext Context(ElapsedTime, true, false);
    CurrentAsset->GetAnimationPose(Pose, Context);

    for (int32 i = 0; i < SkeletalMesh->GetRenderData().Bones.Num(); ++i)
    {
        const FTransform& BoneTransform = Pose.Pose.BoneTransforms[i];
        FMatrix TransformMatrix = JungleMath::CreateModelMatrix(
            BoneTransform.GetLocation(),
            BoneTransform.GetRotation(),
            BoneTransform.GetScale()
        );
        SkeletalMesh->GetRenderData().Bones[i].LocalTransform = TransformMatrix;
    }
}
