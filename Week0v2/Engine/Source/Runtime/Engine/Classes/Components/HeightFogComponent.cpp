#include "HeightFogComponent.h"

UHeightFogComponent::UHeightFogComponent()
    : Super()
    , FogColor(FVector(0.7f, 0.2f, 0.2f))
    , FogDensity(1.0f)
    , FogStart(30.0f)
    , FogEnd(100.0f)
    , FogBaseHeight(5.0f)
    , HeightFallOff(0.02f)
    , bIsHeightFog(true)
    , ScatteringIntensity(1.0f)
    , LightShaftDensity(1.0f)
{
}

UHeightFogComponent::UHeightFogComponent(const UHeightFogComponent& Other)
    : Super(Other)
    , FogColor(Other.GetFogColor())
    , FogDensity(Other.GetFogDensity())
    , FogStart(Other.GetFogStart())
    , FogEnd(Other.GetFogEnd())
    , FogBaseHeight(Other.GetFogBaseHeight())
    , HeightFallOff(Other.GetHeightFallOff())
    , bIsHeightFog(Other.IsHeightFog())
    , ScatteringIntensity(Other.GetScatteringIntensity())
    , LightShaftDensity(Other.GetLightShaftDensity())
{
}

void UHeightFogComponent::SetFogColor(const FVector& InColor) { FogColor = InColor; }
void UHeightFogComponent::SetFogDensity(float InDensity) { FogDensity = InDensity; }
void UHeightFogComponent::SetFogStart(float InStart) { FogStart = InStart; }
void UHeightFogComponent::SetFogEnd(float InEnd) { FogEnd = InEnd; }
void UHeightFogComponent::SetFogBaseHeight(float InHeight) { FogBaseHeight = InHeight; }
void UHeightFogComponent::SetHeightFallOff(float InFalloff) { HeightFallOff = InFalloff; }
void UHeightFogComponent::SetHeightFog(bool bEnabled) { bIsHeightFog = bEnabled; }
void UHeightFogComponent::SetScatteringIntensity(float InIntensity) { ScatteringIntensity = InIntensity; }
void UHeightFogComponent::SetLightShaftDensity(float InDensity) { LightShaftDensity = InDensity; }

void UHeightFogComponent::LoadAndConstruct(const FActorComponentInfo& Info)
{
    Super::LoadAndConstruct(Info);
    const FHeightFogComponentInfo& HeightFogInfo = static_cast<const FHeightFogComponentInfo&>(Info);
    FogColor = HeightFogInfo.FogColor;
    FogDensity = HeightFogInfo.FogDensity;
    FogStart = HeightFogInfo.FogStart;
    FogEnd = HeightFogInfo.FogEnd;
    FogBaseHeight = HeightFogInfo.FogBaseHeight;
    HeightFallOff = HeightFogInfo.HeightFallOff;
    bIsHeightFog = HeightFogInfo.bIsHeightFog;
    ScatteringIntensity = HeightFogInfo.ScatteringIntensity;
    LightShaftDensity = HeightFogInfo.LightShaftDensity;
}

std::shared_ptr<FActorComponentInfo> UHeightFogComponent::GetActorComponentInfo()
{
    std::shared_ptr<FHeightFogComponentInfo> Info = std::make_shared<FHeightFogComponentInfo>();
    Super::GetActorComponentInfo()->Copy(*Info);
    Info->FogColor = FogColor;
    Info->FogDensity = FogDensity;
    Info->FogStart = FogStart;
    Info->FogEnd = FogEnd;
    Info->FogBaseHeight = FogBaseHeight;
    Info->HeightFallOff = HeightFallOff;
    Info->bIsHeightFog = bIsHeightFog;
    Info->ScatteringIntensity = ScatteringIntensity;
    Info->LightShaftDensity = LightShaftDensity;
    return Info;

}
