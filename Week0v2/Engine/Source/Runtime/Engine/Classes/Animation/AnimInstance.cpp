#include "AnimInstance.h"
#include "Components/PrimitiveComponents/MeshComponents/SkeletalMeshComponent.h"

AActor* UAnimInstance::GetOwningActor() const
{
    return GetOwningComponent()->GetOwner();
}

USkeletalMeshComponent* UAnimInstance::GetOwningComponent() const
{
    return OwningComponent;
}

void UAnimInstance::NativeUpdateAnimation(float DeltaSeconds) const
{
    
}
