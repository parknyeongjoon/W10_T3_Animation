#include "AnimSingleNodeInstance.h"
#include "Components/PrimitiveComponents/MeshComponents/SkeletalMeshComponent.h"

UAnimSingleNodeInstance::UAnimSingleNodeInstance(const UAnimSingleNodeInstance& Other)
    :UAnimInstance(Other),
    CurrentAsset(Other.CurrentAsset),
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
}
