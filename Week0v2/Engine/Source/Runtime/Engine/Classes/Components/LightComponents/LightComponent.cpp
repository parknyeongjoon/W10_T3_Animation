#include "LightComponent.h"

ULightComponent::ULightComponent()
    : Super()
{
}

ULightComponent::ULightComponent(const ULightComponent& Other)
    : Super(Other)
{
}

ULightComponent::~ULightComponent()
{
}

void ULightComponent::UninitializeComponent()
{
    Super::UninitializeComponent();
}

void ULightComponent::OnComponentDestroyed()
{
    Super::OnComponentDestroyed();
}

void ULightComponent::DestroyComponent()
{
    Super::DestroyComponent();
}

void ULightComponent::OnRegister()
{
    Super::OnRegister();
}

void ULightComponent::OnUnregister()
{
    Super::OnUnregister();
}

void ULightComponent::TickComponent(float DeltaTime)
{
    Super::TickComponent(DeltaTime);
}

int ULightComponent::CheckRayIntersection(FVector& rayOrigin, FVector& rayDirection, float& pfNearHitDistance)
{
    return Super::CheckRayIntersection(rayOrigin, rayDirection, pfNearHitDistance);
}

void ULightComponent::InitializeComponent()
{
    Super::InitializeComponent();
}

void ULightComponent::BeginPlay()
{
    Super::BeginPlay();
}

void ULightComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
}

UObject* ULightComponent::Duplicate() const
{
    return Super::Duplicate();
}

void ULightComponent::DuplicateSubObjects(const UObject* Source)
{
    Super::DuplicateSubObjects(Source);
}

void ULightComponent::PostDuplicate()
{
    Super::PostDuplicate();
}

std::shared_ptr<FActorComponentInfo> ULightComponent::GetActorComponentInfo()
{
    return Super::GetActorComponentInfo();
}

void ULightComponent::LoadAndConstruct(const FActorComponentInfo& Info)
{
    Super::LoadAndConstruct(Info);
}
