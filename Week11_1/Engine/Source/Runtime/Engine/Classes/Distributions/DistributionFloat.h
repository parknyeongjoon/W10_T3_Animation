#pragma once

#include "Distribution.h"

// UDistribution을 넣어서 생성 방법을 지정할 수 있습니다.
struct FRawDistributionFloat : public FRawDistribution
{
    DECLARE_STRUCT(FRawDistributionFloat)
private:
    //UPROPERTY(
    //    EditAnywhere,
    //    float,
    //    MinValue,
    //    = 0.0f
    //)
    //UPROPERTY(
    //    EditAnywhere,
    //    float,
    //    MaxValue,
    //    = 1.0f
    //)

public:
    UPROPERTY(
        EditAnywhere,
        class UDistributionFloat*,
        Distribution,
        = nullptr
    )

    FRawDistributionFloat()
        //: MinValue(0.0f)
        //, MaxValue(1.0f)
        : Distribution(nullptr)
    {
    }

    float GetValue(struct FRandomStream* InRandomStream = nullptr) const;

    //void Initialize();
};


class UDistributionFloat : public UDistribution
{
    DECLARE_CLASS(UDistributionFloat, UDistribution)

public:
    UDistributionFloat() {}

    virtual float GetValue(struct FRandomStream* InRandomStream = nullptr) const;
};

class UDistributionFloatConstant : public UDistributionFloat
{
    DECLARE_CLASS(UDistributionFloatConstant, UDistributionFloat)

public:
    UDistributionFloatConstant() {}
    virtual float GetValue(struct FRandomStream* InRandomStream = nullptr) const override;

private:
    UPROPERTY(
        EditAnywhere,
        float,
        Constant,
        = 0.0f
    )
};

class UDistributionFloatUniform : public UDistributionFloat
{
    DECLARE_CLASS(UDistributionFloatUniform, UDistributionFloat)

public:
    UDistributionFloatUniform() {}

    virtual float GetValue(struct FRandomStream* InRandomStream = nullptr) const override;

    UPROPERTY(
        EditAnywhere,
        float,
        MinValue,
        = 0.0f
    )

    UPROPERTY(
        EditAnywhere,
        float,
        MaxValue,
        = 1.0f
    )
};