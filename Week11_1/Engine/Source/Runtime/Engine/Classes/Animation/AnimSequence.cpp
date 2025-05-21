#include "AnimSequence.h"
#include "CoreUObject/UObject/Casts.h"

UObject* UAnimSequence::Duplicate(UObject* InOuter)
{
    UAnimSequence* NewComp = Cast<ThisClass>(Super::Duplicate(InOuter));
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
