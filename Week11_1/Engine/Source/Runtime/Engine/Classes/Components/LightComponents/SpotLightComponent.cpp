#include "SpotLightComponent.h"
#include "UObject/ObjectFactory.h"
#include "CoreUObject/UObject/Casts.h"
#include "Math/JungleMath.h"
#include <D3D11RHI/GraphicDevice.h>
#include "EditorEngine.h"
#include "LaunchEngineLoop.h"



USpotLightComponent::~USpotLightComponent()
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

UObject* USpotLightComponent::Duplicate(UObject* InOuter)
{
    USpotLightComponent* NewComp = Cast<ThisClass>(Super::Duplicate(InOuter));
    NewComp->DuplicateSubObjects(this, InOuter);
    NewComp->PostDuplicate();

    return NewComp;
}

void USpotLightComponent::DuplicateSubObjects(const UObject* Source, UObject* InOuter)
{
    Super::DuplicateSubObjects(Source, InOuter);
}

void USpotLightComponent::PostDuplicate()
{
}

FMatrix USpotLightComponent::GetViewMatrix() const
{
    FVector Up = FVector{ 0.0f, 0.0f, 1.0f };
    FVector Forward = GetWorldForwardVector();
    float dot = abs(Up.Dot(Forward));
    if (dot > 0.99f)
        Up = FVector(1.0f, 0.0f, 0.0f);
    return JungleMath::CreateViewMatrix(GetWorldLocation(), Forward + GetWorldLocation(), Up);
} 

FMatrix USpotLightComponent::GetProjectionMatrix() const
{
    return JungleMath::CreateProjectionMatrix(
        OuterConeAngle * 2.0f,
        1.0f,
        0.1f,
        1000.0f
    );
}

std::unique_ptr<FActorComponentInfo> USpotLightComponent::GetComponentInfo()
{
    auto Info = std::make_unique<FSpotlightComponentInfo>();
    SaveComponentInfo(*Info);
    
    return Info;
}


void USpotLightComponent::SaveComponentInfo(FActorComponentInfo& OutInfo)
{
    FSpotlightComponentInfo* Info = static_cast<FSpotlightComponentInfo*>(&OutInfo);
    Super::SaveComponentInfo(*Info);

    Info->InnerConeAngle = InnerConeAngle;
    Info->OuterConeAngle = OuterConeAngle;

}

void USpotLightComponent::LoadAndConstruct(const FActorComponentInfo& Info)
{
    Super::LoadAndConstruct(Info);
    const FSpotlightComponentInfo& PointLightInfo = static_cast<const FSpotlightComponentInfo&>(Info);
    InnerConeAngle = PointLightInfo.InnerConeAngle;
    OuterConeAngle = PointLightInfo.OuterConeAngle;
}

FVector4 USpotLightComponent::GetLightAtlasUV() const
{
    if (!ShadowResource || ShadowResource->GetAtlasSlotIndex() == -1)
        return FVector4();

    int AtlasSlotIndex = ShadowResource->GetAtlasSlotIndex();
    return FVector4(
        (AtlasSlotIndex % 4) * 0.25f,
        (AtlasSlotIndex / 4) * 0.25f,
        0.25f,
        0.25f
    );
}