#pragma once
#include "CurveBase.h"
#include "Container/Map.h"
#include "UObject/ObjectMacros.h"

class UCurveFloat : public UCurveBase
{
    DECLARE_CLASS(UCurveFloat, UCurveBase)
public:
    UCurveFloat();
    virtual ~UCurveFloat();
public:
    TMap<float, float> Keys;

    void AddKey(float Time, float Value);

    virtual float GetFloatValue(float Time) const override;
    virtual float GetStartTime() const override;
    virtual float GetEndTime() const override;
};


