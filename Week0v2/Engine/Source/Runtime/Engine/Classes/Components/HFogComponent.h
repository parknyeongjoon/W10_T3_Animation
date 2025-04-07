#pragma once
#include "PrimitiveComponent.h"

class UHFogComponent: public UPrimitiveComponent
{
    DECLARE_CLASS(UHFogComponent, UPrimitiveComponent);
public:
    UHFogComponent();

    bool bIsActive;
    FVector4 FogColor;
    float FogDensity;
    float FogStartHeight;
    float FogEndHeight;
};
