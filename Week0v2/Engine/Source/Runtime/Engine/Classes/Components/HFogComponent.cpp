#include "HFogComponent.h"

UHFogComponent::UHFogComponent()
{
    this->bIsActive = true;
    this->FogColor = FVector4(1.f, 1.f, 1.f, 1.f);
    this->FogDensity = 1.0f;
    this->FogStartHeight = 0.0f;
    this->FogEndHeight = 10.0f;
}

