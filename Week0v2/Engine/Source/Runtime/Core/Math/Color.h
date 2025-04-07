#pragma once
#include "MathUtility.h"
#include "HAL/PlatformType.h"

struct FVector;

struct FLinearColor
{
    float R, G, B, A;

    static float PowOneOver255Table[256];

    FORCEINLINE FLinearColor() = default;
    FORCEINLINE FLinearColor(float R, float G, float B, float A = 1.0f) : R(R), G(G), B(B), A(A) {}
    FLinearColor(const class FColor& C);
    FLinearColor(const FVector& Vector);

    FORCEINLINE float& Component(int32 Index)
    {
        return (&R)[Index];
    }

    FORCEINLINE const float& Component(int32 Index) const
    {
        return (&R)[Index];
    }
    
	FORCEINLINE FLinearColor operator+(const FLinearColor& ColorB) const
	{
		return FLinearColor(
			this->R + ColorB.R,
			this->G + ColorB.G,
			this->B + ColorB.B,
			this->A + ColorB.A
			);
	}
	FORCEINLINE FLinearColor& operator+=(const FLinearColor& ColorB)
	{
		R += ColorB.R;
		G += ColorB.G;
		B += ColorB.B;
		A += ColorB.A;
		return *this;
	}

	FORCEINLINE FLinearColor operator-(const FLinearColor& ColorB) const
	{
		return FLinearColor(
			this->R - ColorB.R,
			this->G - ColorB.G,
			this->B - ColorB.B,
			this->A - ColorB.A
			);
	}
	FORCEINLINE FLinearColor& operator-=(const FLinearColor& ColorB)
	{
		R -= ColorB.R;
		G -= ColorB.G;
		B -= ColorB.B;
		A -= ColorB.A;
		return *this;
	}

	FORCEINLINE FLinearColor operator*(const FLinearColor& ColorB) const
	{
		return FLinearColor(
			this->R * ColorB.R,
			this->G * ColorB.G,
			this->B * ColorB.B,
			this->A * ColorB.A
			);
	}
	FORCEINLINE FLinearColor& operator*=(const FLinearColor& ColorB)
	{
		R *= ColorB.R;
		G *= ColorB.G;
		B *= ColorB.B;
		A *= ColorB.A;
		return *this;
	}

	FORCEINLINE FLinearColor operator*(float Scalar) const
	{
		return FLinearColor(
			this->R * Scalar,
			this->G * Scalar,
			this->B * Scalar,
			this->A * Scalar
			);
	}

	FORCEINLINE FLinearColor& operator*=(float Scalar)
	{
		R *= Scalar;
		G *= Scalar;
		B *= Scalar;
		A *= Scalar;
		return *this;
	}

	FORCEINLINE FLinearColor operator/(const FLinearColor& ColorB) const
	{
		return FLinearColor(
			this->R / ColorB.R,
			this->G / ColorB.G,
			this->B / ColorB.B,
			this->A / ColorB.A
			);
	}
	FORCEINLINE FLinearColor& operator/=(const FLinearColor& ColorB)
	{
		R /= ColorB.R;
		G /= ColorB.G;
		B /= ColorB.B;
		A /= ColorB.A;
		return *this;
	}

	FORCEINLINE FLinearColor operator/(float Scalar) const
	{
		const float	InvScalar = 1.0f / Scalar;
		return FLinearColor(
			this->R * InvScalar,
			this->G * InvScalar,
			this->B * InvScalar,
			this->A * InvScalar
			);
	}
	FORCEINLINE FLinearColor& operator/=(float Scalar)
	{
		const float	InvScalar = 1.0f / Scalar;
		R *= InvScalar;
		G *= InvScalar;
		B *= InvScalar;
		A *= InvScalar;
		return *this;
	}

	/** Comparison operators */
	FORCEINLINE bool operator==(const FLinearColor& ColorB) const
	{
		return this->R == ColorB.R && this->G == ColorB.G && this->B == ColorB.B && this->A == ColorB.A;
	}
	FORCEINLINE bool operator!=(const FLinearColor& Other) const
	{
		return this->R != Other.R || this->G != Other.G || this->B != Other.B || this->A != Other.A;
	}
    
	FLinearColor CopyWithNewOpacity(float NewOpacity) const
	{
		FLinearColor NewCopy = *this;
		NewCopy.A = NewOpacity;
		return NewCopy;
	}
    /** Converts a linear space RGB color to an HSV color */
    FLinearColor LinearRGBToHSV() const;

    /** Converts an HSV color to a linear space RGB color */
    FLinearColor HSVToLinearRGB() const;

    /** Quantizes the linear color and returns the result as a FColor.  This bypasses the SRGB conversion. */
    FColor Quantize() const;

    /** Quantizes the linear color and returns the result as a FColor with optional sRGB conversion and quality as goal. */
    FColor ToFColor(const bool bSRGB) const;

    
    static const FLinearColor Yellow;
    static const FLinearColor Blue;
    static const FLinearColor Green;
    static const FLinearColor Red;
    static const FLinearColor Transparent;
    static const FLinearColor Black;
    static const FLinearColor Gray;
    static const FLinearColor White;
};

class FColor
{
public:
    union
    {
        struct
        {
            uint8 A, R, G, B;
        };

        uint32 AlignmentDummy;
    };

    uint32& DWColor(void)
    {
        return *((uint32*)this);
    }
    const uint32& DWColor(void) const
    {
        return *((uint32*)this);
    }

    FORCEINLINE FColor() = default;
    FORCEINLINE FColor(uint8 InR, uint8 InG, uint8 InB, uint8 InA = 255)
    {
        R = InR;
        G = InG;
        B = InB;
        A = InA;
    }

    FColor(const FLinearColor& C)
    // put these into the body for proper ordering with INTEL vs non-INTEL_BYTE_ORDER
    {
        R = FMath::Clamp(FMath::Trunc(FMath::Pow(C.R,1.0f / 2.2f) * 255.0f),0,255);
        G = FMath::Clamp(FMath::Trunc(FMath::Pow(C.G,1.0f / 2.2f) * 255.0f),0,255);
        B = FMath::Clamp(FMath::Trunc(FMath::Pow(C.B,1.0f / 2.2f) * 255.0f),0,255);
        A = FMath::Clamp(FMath::Trunc(C.A * 255.0f),0,255);
    }
    
    FORCEINLINE explicit FColor(uint32 InColor)
    {
        DWColor() = InColor;
    }
    
    // Operators.
    FORCEINLINE bool operator==( const FColor &C ) const
    {
        return DWColor() == C.DWColor();
    }

    FORCEINLINE bool operator!=( const FColor& C ) const
    {
        return DWColor() != C.DWColor();
    }

    FORCEINLINE void operator+=(const FColor& C)
    {
        R = (uint8) FMath::Min((int32) R + (int32) C.R,255);
        G = (uint8) FMath::Min((int32) G + (int32) C.G,255);
        B = (uint8) FMath::Min((int32) B + (int32) C.B,255);
        A = (uint8) FMath::Min((int32) A + (int32) C.A,255);
    }


    static const FColor White;
    static const FColor Black;
    static const FColor Red;
    static const FColor Green;
    static const FColor Blue;
    static const FColor Yellow;
    static const FColor Cyan;
    static const FColor Magenta;
};