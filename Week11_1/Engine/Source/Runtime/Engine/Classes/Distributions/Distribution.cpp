#include "Distribution.h"

#include "Math/RandomStream.h"

#include "DistributionFloat.h"
#include "DistributionVector.h"

// struct
//void FRawDistributionFloat::Initialize()
//{
//    if (!Distribution)
//    {
//        Distribution = FObjectFactory::ConstructObject<UDistributionFloat>(nullptr);
//    }
//}

float FRawDistributionFloat::GetValue(struct FRandomStream* InRandomStream) const
{
    if (Distribution)
    {
        return Distribution->GetValue(InRandomStream);
    }
    assert(!Distribution);
    return 0.0f;
}

//void FRawDistributionVector::Initialize()
//{
//    if (!Distribution)
//    {
//        Distribution = FObjectFactory::ConstructObject<UDistributionVector>(nullptr);
//    }
//}

FVector FRawDistributionVector::GetValue(struct FRandomStream* InRandomStream) const
{
    if (Distribution)
    {
        return Distribution->GetValue(InRandomStream);
    }
    assert(!Distribution);
    return FVector::ZeroVector;
}

const float UDistribution::DefaultValue = 1.2345E-20f;

float UDistributionFloat::GetValue(struct FRandomStream* InRandomStream) const
{
    return 0.0f;
}

void UDistributionFloat::SetValue(float NewValue)
{
}

float UDistributionFloatConstant::GetValue(struct FRandomStream* InRandomStream) const
{
    return Constant;
}

void UDistributionFloatConstant::SetValue(float NewValue)
{
    Constant = NewValue;
}

float UDistributionFloatUniform::GetValue(struct FRandomStream* InRandomStream) const
{
    if (InRandomStream)
    {
        return InRandomStream->GetFraction() * (MaxValue - MinValue) + MinValue;
    }
    else
    {
        InRandomStream = FRandomStream::GetDefaultStream();
        return InRandomStream->GetFraction() * (MaxValue - MinValue) + MinValue;
    }
}

FVector UDistributionVector::GetValue(struct FRandomStream* InRandomStream) const
{
    return FVector::ZeroVector;
}

FVector UDistributionVectorConstant::GetValue(struct FRandomStream* InRandomStream) const
{
    return Constant;
}

FVector UDistributionVectorUniform::GetValue(struct FRandomStream* InRandomStream) const
{
    if (InRandomStream)
    {
        return FVector(
            InRandomStream->GetFraction() * (MaxValue.X - MinValue.X) + MinValue.X,
            InRandomStream->GetFraction() * (MaxValue.Y - MinValue.Y) + MinValue.Y,
            InRandomStream->GetFraction() * (MaxValue.Z - MinValue.Z) + MinValue.Z
        );
    }
    else
    {
        InRandomStream = FRandomStream::GetDefaultStream();
        return FVector(
            InRandomStream->GetFraction() * (MaxValue.X - MinValue.X) + MinValue.X,
            InRandomStream->GetFraction() * (MaxValue.Y - MinValue.Y) + MinValue.Y,
            InRandomStream->GetFraction() * (MaxValue.Z - MinValue.Z) + MinValue.Z
        );
    }
}