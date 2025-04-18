#include "SpotLightComponent.h"
#include "UObject/ObjectFactory.h"
#include "CoreUObject/UObject/Casts.h"
#include "Math/JungleMath.h"

USpotLightComponent::USpotLightComponent()
    : Super()
{

}

USpotLightComponent::USpotLightComponent(const USpotLightComponent& Other)
    : Super(Other)
    , InnerConeAngle(Other.InnerConeAngle)
    , OuterConeAngle(Other.OuterConeAngle)
{

}

void USpotLightComponent::SetInnerConeAngle(float Angle)
{
    Angle = JungleMath::DegToRad(Angle);
    InnerConeAngle = Angle;
}

void USpotLightComponent::SetOuterConeAngle(float Angle)
{
    Angle = JungleMath::DegToRad(Angle);
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