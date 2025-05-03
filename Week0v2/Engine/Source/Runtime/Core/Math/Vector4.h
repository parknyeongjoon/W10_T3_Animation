#pragma once
#include "Serialization/Archive.h"

// 4D Vector
struct FVector4 {
    float X, Y, Z, W;
    FVector4(float _x = 0, float _y = 0, float _z = 0, float _a = 0) : X(_x), Y(_y), Z(_z), W(_a) {}
    FVector4(FVector v, float w) : X(v.X), Y(v.Y), Z(v.Z), W(w) {}

    FVector4 operator-(const FVector4& other) const {
        return FVector4(X - other.X, Y - other.Y, Z - other.Z, W - other.W);
    }
    FVector4 operator+(const FVector4& other) const {
        return FVector4(X + other.X, Y + other.Y, Z + other.Z, W + other.W);
    }
    FVector4 operator/(float scalar) const
    {
        return FVector4{ X / scalar, Y / scalar, Z / scalar, W / scalar };
    }
    FVector xyz() const { return FVector(X, Y, Z); }
    void Serialize(FArchive& Ar) const
    {
        Ar << X << Y << Z << W;
    }
    void Deserialize(FArchive& Ar)
    {
        Ar >> X >> Y >> Z >> W;
    }
};
