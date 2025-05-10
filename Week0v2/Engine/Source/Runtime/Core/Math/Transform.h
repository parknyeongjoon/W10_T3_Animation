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
    
private:
    FQuat Rotation;
    FVector Location;
    FVector Scale;
};

inline const FTransform FTransform::Identity = FTransform();