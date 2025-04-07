#pragma once
#include "PrimitiveComponent.h"
#include "Math/Color.h"
#include "Runtime/CoreUObject//UObject//ObjectMacros.h"

class UHeightFogComponent : public UPrimitiveComponent
{
    DECLARE_CLASS(UHeightFogComponent, UPrimitiveComponent)
public:
    UHeightFogComponent();
    bool bIsActive;
    float FogDensity;
    float FogHeightFalloff;
    float HeightOffset;
    float HeightFogStart;
    float HeightFogEnd;
    float FogCutoffDistance;
    float FogMaxOpacity;

    FLinearColor FogInscatteringColor;
    FLinearColor DirectionalInscatteringColor;
    float DirectionalInscatteringExponent;
    float DirectionalInscatteringStartDistance;
};


