#include "RotatingMovementComponent.h"

#include "Components/PrimitiveComponent.h"
#include "UObject/Casts.h"
#include "UObject/ObjectFactory.h"

URotatingMovementComponent::URotatingMovementComponent()
{
    RotationRate.z = 180.0f;
}

URotatingMovementComponent::URotatingMovementComponent(const URotatingMovementComponent& other)
    : RotationRate(other.RotationRate)
{
    
}

void URotatingMovementComponent::TickComponent(float DeltaTime)
{
    const FVector OldRotation = UpdatedComponent->GetWorldRotation();
    const FVector DeltaRotation = (RotationRate * DeltaTime);
    const FVector NewRotation = OldRotation + DeltaRotation;

    UpdatedComponent->SetRotation(NewRotation);
}

UObject* URotatingMovementComponent::Duplicate() const
{
    URotatingMovementComponent* NewComp = FObjectFactory::ConstructObjectFrom<URotatingMovementComponent>(this);
    NewComp->DuplicateSubObjects(this);
    NewComp->PostDuplicate();
    return NewComp;
}

void URotatingMovementComponent::DuplicateSubObjects(const UObject* Source)
{
    UMovementComponent::DuplicateSubObjects(Source);

    URotatingMovementComponent* SourceComp = Cast<URotatingMovementComponent>(Source);
    if (SourceComp)
    {
        RotationRate = SourceComp->RotationRate;
    }
}

void URotatingMovementComponent::PostDuplicate()
{
    UMovementComponent::PostDuplicate();
}