#pragma once

#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"

/** Lookup table for distributions. */
//struct FDistributionLookupTable
//{
//    DECLARE_STRUCT(FDistributionLookupTable)
//
//    UPROPERTY (
//        EditAnywhere,
//        float,
//        TimeScale,
//        = 1.0f
//    )
//
//    UPROPERTY(
//        EditAnywhere,
//        float,
//        TimeBias,
//        = 0.0f
//    )
//
//    UPROPERTY(
//        TArray<float>,
//        Values
//    )
//};

// runtime에 빠르게 sample할 수 있도록 table을 이용
struct FRawDistribution
{
    DECLARE_STRUCT(FRawDistribution)
    //UPROPERTY(
    //    FDistributionLookupTable,
    //    Table
    //)

};

class UDistribution : public UObject
{
    DECLARE_CLASS(UDistribution, UObject)

public:
    UDistribution() = default;

    static const float DefaultValue;
};