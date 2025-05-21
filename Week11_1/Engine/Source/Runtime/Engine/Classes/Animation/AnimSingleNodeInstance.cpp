#include "AnimSingleNodeInstance.h"
#include "Components/PrimitiveComponents/MeshComponents/SkeletalMeshComponent.h"
#include "Animation/AnimSequence.h"
#include "Animation/AnimData/AnimDataModel.h"
#include <Math/JungleMath.h>
#include "CoreUObject/UObject/Casts.h"

UAnimSingleNodeInstance::UAnimSingleNodeInstance()
    :PlayRate(1.f),
    bLooping(true),
    bPlaying(false),
    bReverse(false),
    PreviousTime(0.f),
    LoopStartFrame(0),
    LoopEndFrame(0),
    CurrentKey(0)
{
    CurrentSequence = FObjectFactory::ConstructObject<UAnimSequence>(this);
}

UObject* UAnimSingleNodeInstance::Duplicate(UObject* InOuter)
{
    UAnimSingleNodeInstance* NewComp = Cast<ThisClass>(Super::Duplicate(InOuter));
    NewComp->DuplicateSubObjects(this, InOuter);
    NewComp->PostDuplicate();
    return NewComp;
}

void UAnimSingleNodeInstance::SetAnimationAsset(UAnimSequence* NewAsset, bool bIsLooping, float InPlayRate)
{
    if (NewAsset != CurrentSequence)
    {
        CurrentSequence = NewAsset;
    }

    USkeletalMeshComponent* MeshComponent = GetOwningComponent();
    if (MeshComponent)
    {
        if (MeshComponent->GetSkeletalMesh() == nullptr)
        {
            CurrentSequence = nullptr;
        }
    }

    bLooping = bIsLooping;
    PlayRate = InPlayRate;
    CurrentTime = 0.f;
}

void UAnimSingleNodeInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    USkeletalMeshComponent* SkeletalMeshComp = GetOwningComponent();
    USkeletalMesh* SkeletalMesh = SkeletalMeshComp->GetSkeletalMesh();
    if (!CurrentSequence ||!SkeletalMesh||!SkeletalMesh->GetSkeleton()) return;

    const UAnimDataModel* DataModel = CurrentSequence->GetDataModel();
    if (!DataModel) return;
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
            if (!bReverse&&CurrentTime == EndTime)
            {
                CurrentTime = StartTime;
            }
            else if (bReverse && CurrentTime == StartTime)
            {
                CurrentTime = EndTime;
            }   
        }

        PreviousTime = CurrentTime;
        CurrentTime += DeltaPlayTime;
        CurrentKey = static_cast<int32>(CurrentTime * FrameRate);
        //CurrentSequence->EvaluateAnimNotify()


        if (bLooping)
        {
            if (CurrentTime > EndTime)
            {
                CurrentTime = StartTime + FMath::Fmod(CurrentTime - StartTime, EndTime - StartTime);
                CurrentSequence->ResetNotifies();
            }
            else if (CurrentTime <= StartTime)
            {
                CurrentTime = EndTime - FMath::Fmod(EndTime - CurrentTime, EndTime - StartTime);
            }
        }
        else
        {
            if (!bReverse && CurrentTime >= EndTime)
            {
                CurrentTime = EndTime;
                bPlaying = false;
            }
            else if (bReverse && CurrentTime <= StartTime)
            {
                CurrentTime = StartTime;
                bPlaying = false;
            }
        }
    }
    else
    {
        PreviousTime = CurrentTime;
        CurrentTime = static_cast<float>(CurrentKey) / static_cast<float>(FrameRate);
    }

    FPoseContext Pose;
    FAnimExtractContext Context(CurrentTime, true, false);
    CurrentSequence->GetAnimationPose(Pose, Context);

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
