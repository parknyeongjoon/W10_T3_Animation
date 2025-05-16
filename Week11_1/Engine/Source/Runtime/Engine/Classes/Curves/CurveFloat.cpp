#include "CurveFloat.h"
#include <cfloat> // for FLT_MAX

#include "Math/MathUtility.h"

UCurveFloat::UCurveFloat()
{
}

UCurveFloat::~UCurveFloat()
{
}

void UCurveFloat::AddKey(float Time, float Value)
{
    Keys.Add(Time, Value);
}

float UCurveFloat::GetFloatValue(float Time) const
{
    if (Keys.IsEmpty()) return 0.f;

    auto It = Keys.begin();
    float PrevTime = It->Key;
    float PrevValue = It->Value;
    ++It;

    for (; It != Keys.end(); ++It)
    {
        float CurrTime = It->Key;
        float CurrValue = It->Value;

        if (Time < CurrTime)
        {
            float Alpha = (Time - PrevTime) / (CurrTime - PrevTime);
            return FMath::Lerp(PrevValue, CurrValue, Alpha);
        }

        PrevTime = CurrTime;
        PrevValue = CurrValue;
    }

    return PrevValue;
}

float UCurveFloat::GetStartTime() const
{
    float MinTime = FLT_MAX;
    for (const auto& Pair : Keys)
    {
        if (Pair.Key < MinTime)
            MinTime = Pair.Key;
    }
    return Keys.IsEmpty() ? 0.f : MinTime;
}

float UCurveFloat::GetEndTime() const
{
    float MaxTime = -FLT_MAX;
    for (const auto& Pair : Keys)
    {
        if (Pair.Key > MaxTime)
            MaxTime = Pair.Key;
    }
    return Keys.IsEmpty() ? 1.f : MaxTime;
}
