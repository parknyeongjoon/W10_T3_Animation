#include "AnimationAsset.h"
#include "CoreUObject/UObject/Casts.h"

UObject* UAnimationAsset::Duplicate(UObject* InOuter)
{
    UAnimationAsset* NewComp = Cast<ThisClass>(Super::Duplicate(InOuter));
    NewComp->DuplicateSubObjects(this, InOuter);
    NewComp->PostDuplicate();
    return NewComp;
}

void UAnimationAsset::DuplicateSubObjects(const UObject* Source, UObject* InOuter)
{
    UObject::DuplicateSubObjects(Source, InOuter);
}

void UAnimationAsset::PostDuplicate()
{
}
