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
    bool bIsExponential;
    float FogDensity;
    float HeightFogStart;
    float HeightFogEnd;
    float FogMaxOpacity;
    float DistanceFogNear;
    float DistanceFogFar;

    FLinearColor FogInscatteringColor;
    FLinearColor DirectionalInscatteringColor;
    FVector DirectionalLightDirection;
    float DirectionalInscatteringExponent;
    float DirectionalInscatteringStartDistance;
};


