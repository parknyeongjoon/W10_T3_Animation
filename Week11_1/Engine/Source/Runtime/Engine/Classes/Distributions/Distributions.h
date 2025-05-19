#pragma once

#include <cstdlib>

enum class EDistributionFloatMode
{
    EDM_Constant,
    EDM_UniformRandom
};

struct FRawDistributionFloat
{
    EDistributionFloatMode Mode = EDistributionFloatMode::EDM_Constant;

    // Constant 모드용
    float Constant = 1.0f;

    float Min = 0.0f;
    float Max = 1.0f;

    float GetValue() const
    {
        switch (Mode) {
        case EDistributionFloatMode::EDM_Constant:
            return Constant;
            
        case EDistributionFloatMode::EDM_UniformRandom:
            return RandomRange(Min, Max);

        default:
            return 0.0f;
        }
    }

    static float RandomRange(float Min, float Max) {
        float t = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        return Min + t * (Max - Min);
    }

};