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
    UPointLightComponent* MyLightComp = GetOwner()->AddComponent<UPointLightComponent>();
    MyLightComp->SetIntensity(3.f);
    MyLightComp->SetRadius(20.f);
}

void UFireBallComponent::TickComponent(float DeltaTime)
{
    Super::TickComponent(DeltaTime);
}
