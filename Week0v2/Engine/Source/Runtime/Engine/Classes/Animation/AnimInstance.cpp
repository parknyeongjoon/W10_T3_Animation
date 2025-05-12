#include "AnimInstance.h"
#include "Components/PrimitiveComponents/MeshComponents/SkeletalMeshComponent.h"
#include "UObject/Casts.h"

UObject* UAnimInstance::Duplicate(UObject* InOuter)
{
    UAnimInstance* NewComp = FObjectFactory::ConstructObjectFrom<UAnimInstance>(this, InOuter);
    NewComp->DuplicateSubObjects(this, InOuter);
    NewComp->PostDuplicate();
    return NewComp;
}

AActor* UAnimInstance::GetOwningActor() const
{
    return GetOwningComponent()->GetOwner();
}

USkeletalMeshComponent* UAnimInstance::GetOwningComponent() const
{
    return CastChecked<USkeletalMeshComponent>(GetOuter());
}

void UAnimInstance::NativeUpdateAnimation(float DeltaSeconds) const
{
    
}
