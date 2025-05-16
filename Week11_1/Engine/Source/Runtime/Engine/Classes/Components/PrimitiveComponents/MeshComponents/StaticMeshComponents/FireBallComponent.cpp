#include "FireBallComponent.h"

#include "GameFramework/Actor.h"

UFireBallComponent::UFireBallComponent(const UFireBallComponent& Other)
    : Super(Other)
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

UObject* UFireBallComponent::Duplicate(UObject* InOuter)
{
    UFireBallComponent* NewComponent = FObjectFactory::ConstructObjectFrom<UFireBallComponent>(this, InOuter);
    NewComponent->DuplicateSubObjects(this, InOuter);
    NewComponent->PostDuplicate();
    return NewComponent;
}

void UFireBallComponent::DuplicateSubObjects(const UObject* Source, UObject* InOuter)
{
    Super::DuplicateSubObjects(Source, InOuter);
}

void UFireBallComponent::PostDuplicate()
{
    Super::PostDuplicate();
}