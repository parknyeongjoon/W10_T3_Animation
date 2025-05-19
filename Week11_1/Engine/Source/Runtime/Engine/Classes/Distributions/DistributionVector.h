#pragma once

#include "Distribution.h"

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