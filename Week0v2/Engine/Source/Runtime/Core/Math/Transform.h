#pragma once
#include "Quat.h"
#include "Vector.h"
#include "UObject/ObjectMacros.h"

class FTransform
{
public:
    FTransform() : Rotation(FQuat::Identity), Location(FVector::ZeroVector), Scale(FVector::OneVector) {}
    FTransform(FQuat InRotation, FVector InLocation, FVector InScale) : Rotation(InRotation), Location(InLocation), Scale(InScale) {}
    
    PROPERTY(FQuat, Rotation);
    PROPERTY(FVector, Location);
    PROPERTY(FVector, Scale);

    const static FTransform Identity;

    static FTransform Blend(const FTransform& Atom1, const FTransform& Atom2, float Alpha);
    
private:
    FQuat Rotation;
    FVector Location;
    FVector Scale;
};

inline const FTransform FTransform::Identity = FTransform();

inline FTransform FTransform::Blend(const FTransform& Atom1, const FTransform& Atom2, float Alpha)
{
    FTransform Result;

    Result.Rotation = FQuat::Slerp(Atom1.Rotation, Atom2.Rotation, Alpha);
    Result.Location = FMath::Lerp(Atom1.GetLocation(), Atom2.GetLocation(), Alpha);
    Result.Scale = FMath::Lerp(Atom1.Scale, Atom2.Scale, Alpha);

    return Result;
}