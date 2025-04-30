#pragma once
#include "Math/Rotator.h"
#include "Math/Vector.h"
#include "Math/Vector4.h"

class AActor;
class UCameraComponent;

struct FViewInfo
{
    FVector Location = FVector();
    FRotator Rotation = FRotator();
    float FOV = 0.0;

    FViewInfo();
    FViewInfo(const FVector InLocation, const FRotator& InRotation, const float InFOV) : Location(InLocation), Rotation(InRotation), FOV(InFOV) {}
};

struct FTViewTarget
{
    AActor* Target = nullptr;
    FViewInfo ViewInfo = FViewInfo();

    FTViewTarget();
};

struct FPostProcessSettings
{
    uint8 bOverride_ColorGamma = false;
    FVector4 ColorGamma = FVector4();

    FPostProcessSettings();
};