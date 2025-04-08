#include "FireBallComponent.h"

#include "PointLightComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"

UFireBallComponent::UFireBallComponent(const UFireBallComponent& Other): Super(Other)
{
}

UFireBallComponent::~UFireBallComponent()
{
}

void UFireBallComponent::InitializeComponent()
{
    Super::InitializeComponent();

}

void UFireBallComponent::TickComponent(float DeltaTime)
{
    Super::TickComponent(DeltaTime);
}

UObject* UFireBallComponent::Duplicate() const
{
    UFireBallComponent* NewComponent = FObjectFactory::ConstructObjectFrom<UFireBallComponent>(this);
    NewComponent->DuplicateSubObjects(this);
    NewComponent->PostDuplicate();
    return NewComponent;
}

void UFireBallComponent::DuplicateSubObjects(const UObject* Source)
{
    Super::DuplicateSubObjects(Source);
}

void UFireBallComponent::PostDuplicate()
{
    Super::PostDuplicate();
}