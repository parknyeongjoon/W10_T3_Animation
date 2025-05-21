#pragma once
#include <Math/Rotator.h>
#include <Distributions/DistributionFloat.h>
namespace ParticleModuleDefaults
{
    struct Required
    {
        static const FWString TexturePath;
        static const FVector EmitterOrigin;
        static const FRotator EmitterRotation;
        static constexpr float EmitterDuration = 1.0f;
        static constexpr float EmitterDurationLow = 0.0f;
        static constexpr bool bUseDurationRange = false;
        static constexpr bool bUseDelayRange = false;
        static constexpr int32 SubImagesHorizontal = 4;
        static constexpr int32 SubImagesVertical = 1;
    };

    struct Spawn
    {
        static constexpr float Rate = 10.0f;
        static constexpr float RateScale = 1.0f;
        
    };

    struct Velocity
    {
        static const FVector LinearVelocity;
        static constexpr float RadialVelocity = 0.0f;
    };

    struct Color
    {
        static const FVector MinColor; // RGB (0 ~ 1)
        static const FVector MaxColor;
    };

    struct Lifetime
    {
        static constexpr float Min = 0.0f;
        static constexpr float Max = 30.0f;
    };

    struct Location
    {
        static const FVector Min;
        static const FVector Max;
    };

    struct Size
    {
        static const FVector Min;
        static const FVector Max;
    };
}