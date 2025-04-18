#include "RotatingMovementComponent.h"

#include "Components/PrimitiveComponents/PrimitiveComponent.h"
#include "UObject/Casts.h"
#include "UObject/ObjectFactory.h"
#include "Math/JungleMath.h"
URotatingMovementComponent::URotatingMovementComponent()
{
}

URotatingMovementComponent::URotatingMovementComponent(const URotatingMovementComponent& other)
    : RotationRate(other.RotationRate)
{
    
}

void URotatingMovementComponent::TickComponent(float DeltaTime)
{
    const FQuat OldRotation = UpdatedComponent->GetComponentQuat();
    const FQuat DeltaRotation = JungleMath::EulerToQuaternion(RotationRate * DeltaTime);
    const FQuat NewRotation = OldRotation * DeltaRotation;

    UpdatedComponent->SetRelativeQuat(NewRotation);
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

std::shared_ptr<FActorComponentInfo> URotatingMovementComponent::GetActorComponentInfo()
{
    std::shared_ptr<FRotatingMovementComponentInfo> Info = std::make_shared<FRotatingMovementComponentInfo>();
    Super::GetActorComponentInfo()->Copy(*Info);
    Info->RotationRate = RotationRate;
    return Info;
}

void URotatingMovementComponent::LoadAndConstruct(const FActorComponentInfo& Info)
{
    Super::LoadAndConstruct(Info);
    const FRotatingMovementComponentInfo& RotatingMovementInfo = static_cast<const FRotatingMovementComponentInfo&>(Info);
    RotationRate = RotatingMovementInfo.RotationRate;
}
