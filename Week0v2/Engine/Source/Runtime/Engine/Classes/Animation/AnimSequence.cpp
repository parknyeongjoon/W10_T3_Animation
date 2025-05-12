#include "AnimSequence.h"

UAnimSequence::UAnimSequence(const UAnimSequence& Other)
    :UAnimSequenceBase(Other),
    InterpolationType(Other.InterpolationType)
{
}

UObject* UAnimSequence::Duplicate(UObject* InOuter)
{
    UAnimSequence* NewComp = FObjectFactory::ConstructObjectFrom<UAnimSequence>(this, InOuter);
    NewComp->DuplicateSubObjects(this, InOuter);
    NewComp->PostDuplicate();
    return NewComp;
}

void UAnimSequence::DuplicateSubObjects(const UObject* Source, UObject* InOuter)
{
    Super::DuplicateSubObjects(Source, InOuter);
}

void UAnimSequence::PostDuplicate()
{
}
