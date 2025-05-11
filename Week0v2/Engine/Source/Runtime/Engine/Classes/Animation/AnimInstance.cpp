#include "AnimInstance.h"
#include "UObject/Casts.h"

USkeletalMeshComponent* UAnimInstance::GetSkelMeshComponent() const
{
    return nullptr;
    //return CastChecked<USkeletalMeshComponent>(GetOuter());
}

void UAnimInstance::TriggerAnimNotifies(float DeltaSeconds)
{
}

void UAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
}
