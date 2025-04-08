#pragma once
#include "MathUtility.h"
#include "Serialization/Archive.h"

struct FLinearColor
{
    union
    {
        struct
        {
            float R, G, B, A;
        };

        float RGBA[4];
    };

    FLinearColor() : R(0), G(0), B(0), A(0) {}
    FLinearColor(float InR, float InG, float InB, float InA = 1.0f) : R(InR), G(InG), B(InB), A(InA) {}

    FLinearColor operator+(const FLinearColor& ColorB) const
    {
        return FLinearColor(R + ColorB.R, G + ColorB.G, B + ColorB.B, A + ColorB.A);
    }

    FLinearColor operator-(const FLinearColor& ColorB) const
    {
        return FLinearColor(R - ColorB.R, G - ColorB.G, B - ColorB.B, A - ColorB.A);
    }

    FLinearColor operator*(const FLinearColor& ColorB) const
    {
        return FLinearColor(R * ColorB.R, G * ColorB.G, B * ColorB.B, A * ColorB.A);
    }

    FLinearColor operator*(float Scalar) const
    {
        return FLinearColor(R * Scalar, G * Scalar, B * Scalar, A * Scalar);
    }

    FLinearColor operator/(float Scalar) const
    {
        float InvScalar = 1.0f / Scalar;
        return FLinearColor(R * InvScalar, G * InvScalar, B * InvScalar, A * InvScalar);
    }

    bool operator==(const FLinearColor& ColorB) const
    {
        return R == ColorB.R && G == ColorB.G && B == ColorB.B && A == ColorB.A;
    }

    bool operator!=(const FLinearColor& Other) const
    {
        return R != Other.R || G != Other.G || B != Other.B || A != Other.A;
    }

    FLinearColor Clamp(float InMin = 0.0f, float InMax = 1.0f) const
    {
        return FLinearColor(
            FMath::Clamp(R, InMin, InMax),
            FMath::Clamp(G, InMin, InMax),
            FMath::Clamp(B, InMin, InMax),
            FMath::Clamp(A, InMin, InMax)
        );
    }

    static FLinearColor Lerp(const FLinearColor& A, const FLinearColor& B, float Alpha)
    {
        return FLinearColor(
            A.R + Alpha * (B.R - A.R),
            A.G + Alpha * (B.G - A.G),
            A.B + Alpha * (B.B - A.B),
            A.A + Alpha * (B.A - A.A)
        );
    }

    void Serialize(FArchive& Ar) const
    {
        Ar << R << G << B << A;
    }

    void Deserialize(FArchive& Ar)
    {
        Ar >> R >> G >> B >> A;
    }
    static const FLinearColor White;
    static const FLinearColor Black;
    static const FLinearColor Red;
    static const FLinearColor Green;
    static const FLinearColor Blue;
};
