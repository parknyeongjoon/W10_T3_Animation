#include "AnimationAsset.h"

UAnimationAsset::UAnimationAsset(const UAnimationAsset& Other): UObject(Other)
{
}

UObject* UAnimationAsset::Duplicate(UObject* InOuter)
{
    UAnimationAsset* NewComp = FObjectFactory::ConstructObjectFrom<UAnimationAsset>(this, InOuter);
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
