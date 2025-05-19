#pragma once

#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"

/** Lookup table for distributions. */
struct FDistributionLookupTable
{
    DECLARE_STRUCT(FDistributionLookupTable)

    UPROPERTY (
        EditAnywhere,
        float,
        TimeScale,
        = 1.0f
    )

    UPROPERTY(
        EditAnywhere,
        float,
        TimeBias,
        = 0.0f
    )

    UPROPERTY(
        TArray<float>,
        Values
    )
};

// base class for baked out distribution type
struct FRawDistribution
{
    DECLARE_STRUCT(FRawDistribution)

    UPROPERTY(
        FDistributionLookupTable,
        Table
    )
};

class UDistribution : public UObject
{
    DECLARE_ABSTRACT_CLASS(UDistribution, UObject)

public:
    UDistribution() {}

    static const float DefaultValue;
};