#pragma once

#include "Distribution.h"

// UDistribution을 넣어서 생성 방법을 지정할 수 있습니다.
struct FRawDistributionVector : public FRawDistribution
{
    DECLARE_STRUCT(FRawDistributionVector)

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

    //UPROPERTY(
    //    EditAnywhere,
    //    FVector,
    //    MinValue,
    //    = FVector::ZeroVector
    //)
    //UPROPERTY(
    //    EditAnywhere,
    //    FVector,
    //    MaxValue,
    //    = FVector::OneVector
    //)

public:
    UPROPERTY(
        EditAnywhere,
        class UDistributionVector*,
        Distribution,
        = nullptr
    )

    FRawDistributionVector()
        //: MinValue(0.0f)
        //, MaxValue(1.0f)
        : Distribution(nullptr)
    {
    }

    FVector GetValue(struct FRandomStream* InRandomStream = nullptr) const;
    //void Initialize();
};


class UDistributionVector : public UDistribution
{
    DECLARE_CLASS(UDistributionVector, UDistribution)

public:
    UDistributionVector() {}
    virtual FVector GetValue(struct FRandomStream* InRandomStream = nullptr) const;
};

class UDistributionVectorConstant : public UDistributionVector
{
    DECLARE_CLASS(UDistributionVectorConstant, UDistributionVector)

public:
    UDistributionVectorConstant() {}
    virtual FVector GetValue(struct FRandomStream* InRandomStream = nullptr) const override;

    UPROPERTY(
        EditAnywhere,
        FVector,
        Constant,
        = FVector::ZeroVector
    )

};

class UDistributionVectorUniform : public UDistributionVector
{
    DECLARE_CLASS(UDistributionVectorUniform, UDistributionVector)

public:
    UDistributionVectorUniform() {}
    virtual FVector GetValue(struct FRandomStream* InRandomStream = nullptr) const override;
    UPROPERTY(
        EditAnywhere,
        FVector,
        MinValue,
        = FVector::ZeroVector
    )
    UPROPERTY(
        EditAnywhere,
        FVector,
        MaxValue,
        = FVector::OneVector
    )
};