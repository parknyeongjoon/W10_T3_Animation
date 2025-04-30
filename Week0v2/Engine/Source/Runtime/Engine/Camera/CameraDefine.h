#pragma once
#include "Math/Rotator.h"
#include "Math/Vector.h"
#include "Math/Vector4.h"

class AActor;
class UCameraComponent;

struct FViewInfo
{
    FVector Location;
    FRotator Rotation;
    float FOV;
};

struct FTViewTarget
{
    AActor* Target;
    FViewInfo ViewInfo = FViewInfo();

    FTViewTarget() : Target(nullptr) {}
};

struct FPostProcessSettings
{
    uint8 bOverride_ColorGamma = false;
    FVector4 ColorGamma = FVector4(1,1,1,1);

    FPostProcessSettings() {}
};