#include "FireBallComponent.h"

#include "PointLightComponent.h"
#include "GameFramework/Actor.h"

UFireBallComponent::UFireBallComponent()
{
    Super();
}

UFireBallComponent::~UFireBallComponent()
{
}

void UFireBallComponent::InitializeComponent()
{
    Super::InitializeComponent();
    GetOwner()->AddComponent<UPointLightComponent>();
}

void UFireBallComponent::TickComponent(float DeltaTime)
{
    Super::TickComponent(DeltaTime);
}
