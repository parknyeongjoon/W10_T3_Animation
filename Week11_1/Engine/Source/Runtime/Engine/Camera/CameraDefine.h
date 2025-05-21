#pragma once
#include "Math/Color.h"
#include "Math/Rotator.h"
#include "Math/Vector.h"
#include "Math/Vector4.h"

class AActor;
class UCameraComponent;

struct FSimpleViewInfo
{
    FVector Location = FVector();
    FRotator Rotation = FRotator();
    float FOV = 0.0;

    FSimpleViewInfo() = default;
    FSimpleViewInfo(const FVector InLocation, const FRotator& InRotation, const float InFOV) : Location(InLocation), Rotation(InRotation), FOV(InFOV) {}
};

struct FTViewTarget
{
    AActor* Target = nullptr;
    FSimpleViewInfo ViewInfo = FSimpleViewInfo();
};

struct FPostProcessSettings
{
    uint8 bOverride_ColorGamma;
    uint8 bOverride_GaussianBlur;
    uint8 bOverride_Fade;
    
    FVector4 ColorGamma;
    
    float GaussianBlurStrength = 0.0f;
    float GaussianBlurRadius = 1.0f;
    
    float FadeAlpha;
    FLinearColor FadeColor;

    float LetterboxSize = 0.0f; 
    float PillarboxSize = 0.0f;
};