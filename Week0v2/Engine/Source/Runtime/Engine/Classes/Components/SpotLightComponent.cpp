#include "SpotLightComponent.h"
#include "UObject/ObjectFactory.h"
#include "CoreUObject/UObject/Casts.h"

USpotLightComponent::USpotLightComponent()
{
}

USpotLightComponent::USpotLightComponent(const USpotLightComponent& Other)
{
}

void USpotLightComponent::SetInnerConeAngle(float Angle)
{
    InnerConeAngle = Angle;
}

void USpotLightComponent::SetOuterConeAngle(float Angle)
{
    OuterConeAngle = Angle;
}

UObject* USpotLightComponent::Duplicate() const
{
    USpotLightComponent* NewComp = FObjectFactory::ConstructObjectFrom<USpotLightComponent>(this);
    NewComp->DuplicateSubObjects(this);
    NewComp->PostDuplicate();

    return NewComp;
}

void USpotLightComponent::DuplicateSubObjects(const UObject* Source)
{
    Super::DuplicateSubObjects(Source);
    USpotLightComponent* SourceComp = Cast<USpotLightComponent>(Source);
    if (SourceComp)
    {
        InnerConeAngle = SourceComp->InnerConeAngle;
        OuterConeAngle = SourceComp->OuterConeAngle;
    }
}

void USpotLightComponent::PostDuplicate()
{
}

std::shared_ptr<FActorComponentInfo> USpotLightComponent::GetActorComponentInfo()
{
    std::shared_ptr<FSpotlightComponentInfo> Info = std::make_shared<FSpotlightComponentInfo>();
    Super::GetActorComponentInfo()->Copy(*Info);

    Info->InnerConeAngle = InnerConeAngle;
    Info->OuterConeAngle = OuterConeAngle;

    return Info;
}

void USpotLightComponent::LoadAndConstruct(const FActorComponentInfo& Info)
{
    Super::LoadAndConstruct(Info);
    const FSpotlightComponentInfo& PointLightInfo = static_cast<const FSpotlightComponentInfo&>(Info);
    InnerConeAngle = PointLightInfo.InnerConeAngle;
    OuterConeAngle = PointLightInfo.OuterConeAngle;
}