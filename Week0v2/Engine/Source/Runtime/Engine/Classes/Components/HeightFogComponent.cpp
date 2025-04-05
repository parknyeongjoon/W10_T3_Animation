#include "HeightFogComponent.h"

UHeightFogComponent::UHeightFogComponent()
    : UPrimitiveComponent()
{
    FogDensity = 0.5f;
    FogHeightFalloff = 0.0f;
    HeightOffset = 0.0f;
    StartDistance = 0.0f;
    FogCutoffDistance = 1000.0f;
    FogMaxOpacity = 0.8f;

    FogInscatteringColor = FLinearColor(1.0f, 0.0f, 0.0f);
    DirectionalInscatteringColor = FLinearColor(1.0f, 0.0f, 0.0f);
    DirectionalInscatteringExponent = 1.0f;
    DirectionalInscatteringStartDistance = 100.0f;
}
