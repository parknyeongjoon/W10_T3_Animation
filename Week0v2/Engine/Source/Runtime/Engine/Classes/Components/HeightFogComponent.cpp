#include "HeightFogComponent.h"

UHeightFogComponent::UHeightFogComponent()
    : UPrimitiveComponent()
{
    bIsActive = true;
    bIsExponential = true;
    FogDensity = 0.5f;
    HeightFogStart = 0.0f;
    HeightFogEnd = 10.0f;
    DistanceFogNear = 0.0f;
    DistanceFogFar = 100.0f;
    FogMaxOpacity = 0.8f;

    FogInscatteringColor = FLinearColor(1.0f, 0.0f, 0.0f);
    DirectionalInscatteringColor = FLinearColor(1.0f, 0.0f, 0.0f);
    DirectionalLightDirection = FVector(0.0f, 0.0f, -1.0f);
    DirectionalInscatteringExponent = 1.0f;
    DirectionalInscatteringStartDistance = 100.0f;
}

void UHeightFogComponent::LoadAndConstruct(const FActorComponentInfo& Info)
{
    Super::LoadAndConstruct(Info);
    const FHeightFogComponentInfo& HeightFogInfo = static_cast<const FHeightFogComponentInfo&>(Info);
    bIsActive = HeightFogInfo.bIsActive;
    bIsExponential = HeightFogInfo.bIsExponential;
    FogDensity = HeightFogInfo.FogDensity;
    HeightFogStart = HeightFogInfo.HeightFogStart;
    HeightFogEnd = HeightFogInfo.HeightFogEnd;
    FogMaxOpacity = HeightFogInfo.FogMaxOpacity;
    DistanceFogNear = HeightFogInfo.DistanceFogNear;
    DistanceFogFar = HeightFogInfo.DistanceFogFar;
    FogInscatteringColor = HeightFogInfo.FogInscatteringColor;
    DirectionalInscatteringColor = HeightFogInfo.DirectionalInscatteringColor;
    DirectionalLightDirection = HeightFogInfo.DirectionalLightDirection;
    DirectionalInscatteringExponent = HeightFogInfo.DirectionalInscatteringExponent;
    DirectionalInscatteringStartDistance = HeightFogInfo.DirectionalInscatteringStartDistance;
}

std::shared_ptr<FActorComponentInfo> UHeightFogComponent::GetActorComponentInfo()
{
    std::shared_ptr<FHeightFogComponentInfo> Info = std::make_shared<FHeightFogComponentInfo>();
    Super::GetActorComponentInfo()->Copy(*Info);
    Info->bIsActive = bIsActive;
    Info->bIsExponential = bIsExponential;
    Info->FogDensity = FogDensity;
    Info->HeightFogStart = HeightFogStart;
    Info->HeightFogEnd = HeightFogEnd;
    Info->FogMaxOpacity = FogMaxOpacity;
    Info->DistanceFogNear = DistanceFogNear;
    Info->DistanceFogFar = DistanceFogFar;
    Info->FogInscatteringColor = FogInscatteringColor;
    Info->DirectionalInscatteringColor = DirectionalInscatteringColor;
    Info->DirectionalLightDirection = DirectionalLightDirection;
    Info->DirectionalInscatteringExponent = DirectionalInscatteringExponent;
    Info->DirectionalInscatteringStartDistance = DirectionalInscatteringStartDistance;
    return Info;

}
