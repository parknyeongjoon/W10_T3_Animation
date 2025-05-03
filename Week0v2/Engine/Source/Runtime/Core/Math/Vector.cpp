#include "Vector.h"

const FVector2D FVector2D::ZeroVector = FVector2D(0, 0);
const FVector2D FVector2D::OneVector = FVector2D(1, 1);

const FVector FVector::ZeroVector = {0.0f, 0.0f, 0.0f};
const FVector FVector::OneVector = {1.0f, 1.0f, 1.0f};
const FVector FVector::ForwardVector = {1.0f, 0.0f, 0.0f};
const FVector FVector::RightVector = {0.0f, 1.0f, 0.0f};
const FVector FVector::UpVector = {0.0f, 0.0f, 1.0f};

const FVector FVector::XAxisVector = FVector(1, 0, 0);
const FVector FVector::YAxisVector = FVector(0, 1, 0);
const FVector FVector::ZAxisVector = FVector(0, 0, 1);


void FVector::Normalize(float Tolerance)
{
    const float SquareSum = X * X + Y * Y + Z * Z;
    if (SquareSum > Tolerance)
    {
        const float Scale = FMath::InvSqrt(SquareSum);
        X *= Scale;
        Y *= Scale;
        Z *= Scale;
    }
    else
    {
        const float Scale = FMath::InvSqrt(3.f);
        X = 1 * Scale;
        Y = 1 * Scale;
        Z = 1 * Scale;
    }
}


FVector FVector::GetSafeNormal(float Tolerance) const
{
    const float SquareSum = X*X + Y*Y + Z*Z;

    // Not sure if it's safe to add tolerance in there. Might introduce too many errors
    if (SquareSum == 1.f)
    {
        return *this;
    }
    else if (SquareSum < Tolerance)
    {
        return ZeroVector;
    }
    const float Scale = FMath::InvSqrt(SquareSum);
    return {X * Scale, Y * Scale, Z * Scale};
}

FVector FVector::GetUnsafeNormal() const
{
    const float Scale = FMath::InvSqrt(X*X + Y*Y + Z*Z);
    return {X * Scale, Y * Scale, Z * Scale};
}
