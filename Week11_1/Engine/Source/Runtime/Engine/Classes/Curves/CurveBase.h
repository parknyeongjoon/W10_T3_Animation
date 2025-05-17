#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
class UCurveBase : public UObject
{
    DECLARE_CLASS(UCurveBase, UObject)
public:
    UCurveBase(){}

    virtual float GetStartTime() const { return 0.f; }
    virtual float GetEndTime() const { return 1.f; }

    virtual float GetFloatValue(float Time) const { return 0.f; } // 오버라이드 전용
};

