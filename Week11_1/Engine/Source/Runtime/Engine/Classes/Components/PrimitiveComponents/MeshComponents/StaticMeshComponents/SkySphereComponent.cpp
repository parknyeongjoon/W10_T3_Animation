#include "SkySphereComponent.h"

USkySphereComponent::USkySphereComponent()
{
}

USkySphereComponent::~USkySphereComponent()
{
}

void USkySphereComponent::InitializeComponent()
{
    Super::InitializeComponent();
}

void USkySphereComponent::TickComponent(float DeltaTime)
{
    UOffset += 0.005f;
    VOffset += 0.005f;
    Super::TickComponent(DeltaTime);
}