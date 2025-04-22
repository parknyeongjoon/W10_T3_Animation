#pragma once

#include <DirectXMath.h>

#include <cmath>

#include "MathUtility.h"

struct FMath;

#include "Serialization/Archive.h"
struct FVector2D
{
	float x,y;
	FVector2D(float _x = 0, float _y = 0) : x(_x), y(_y) {}

	FVector2D operator+(const FVector2D& rhs) const
	{
		return FVector2D(x + rhs.x, y + rhs.y);
	}
	FVector2D operator-(const FVector2D& rhs) const
	{
		return FVector2D(x - rhs.x, y - rhs.y);
	}
	FVector2D operator*(float rhs) const
	{
		return FVector2D(x * rhs, y * rhs);
	}
	FVector2D operator/(float rhs) const
	{
		return FVector2D(x / rhs, y / rhs);
	}
	FVector2D& operator+=(const FVector2D& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		return *this;
	}

    void Serialize(FArchive& Ar) const
    {
        Ar << x << y;
    }
    void Deserialize(FArchive& Ar)
    {
        Ar >> x >> y;
    }
};

// 3D 벡터
struct FVector
{
    float x, y, z;
    FVector(float _x = 0, float _y = 0, float _z = 0) : x(_x), y(_y), z(_z) {}

    FVector operator+(const FVector& Other) const;
    FVector& operator+=(const FVector& Other);

    FVector operator-(const FVector& Other) const;
    FVector& operator-=(const FVector& Other);

    FVector operator*(const FVector& Other) const;
    FVector operator*(float Scalar) const;
    FVector& operator*=(float Scalar);

    FVector operator/(const FVector& Other) const;
    FVector operator/(float Scalar) const;
    FVector& operator/=(float Scalar);

    FVector operator-() const;

    // 벡터 내적
    float Dot(const FVector& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    // 벡터 크기
    float Magnitude() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    float MagnitudeSquared() const
    {
        return x * x + y * y + z * z;
    }

    // 벡터 정규화
    FVector Normalize() const {
        float mag = Magnitude();
        return (mag > 0) ? FVector(x / mag, y / mag, z / mag) : FVector(0, 0, 0);
    }
    FVector Cross(const FVector& Other) const
    {
        return FVector{
            y * Other.z - z * Other.y,
            z * Other.x - x * Other.z,
            x * Other.y - y * Other.x
        };
    }
    FVector Min(const FVector& Other) const
    {
        FVector result = *this;
        if (x > Other.x)
            result.x = Other.x;
        if (y > Other.y)
            result.y = Other.y;
        if (z > Other.z)
            result.z = Other.z;

        return result;
    }

    FVector Max(const FVector& Other) const
    {
        FVector result = *this;
        if (x < Other.x)
            result.x = Other.x;
        if (y < Other.y)
            result.y = Other.y;
        if (z < Other.z)
            result.z = Other.z;

        return result;
    }

    bool operator==(const FVector& other) const {
        return (x == other.x && y == other.y && z == other.z);
    }

    float Distance(const FVector& other) const {
        // 두 벡터의 차 벡터의 크기를 계산
        return ((*this - other).Magnitude());
    }
    DirectX::XMFLOAT3 ToXMFLOAT3() const
    {
        return DirectX::XMFLOAT3(x, y, z);
    }

    FVector ClampMaxSize(float MaxSize) const
    {
        if (MaxSize < 1.e-4f)
        {
            return ZeroVector;
        }

        const float VSq = MagnitudeSquared();
        if (VSq > MaxSize * MaxSize)
        {
            const float Scale = MaxSize * FMath::InvSqrt(VSq);
            return {x * Scale, y * Scale, z * Scale};
        }
        else
        {
            return *this;
        }
    }
    void Serialize(FArchive& Ar) const
    {
        Ar << x << y << z;
    }

    void Deserialize(FArchive& Ar)
    {
        Ar >> x >> y >> z;
    }
    bool IsNearlyZero(float Tolerance= KINDA_SMALL_NUMBER) const
    {
        return FMath::Abs(x) <= Tolerance &&
               FMath::Abs(y) <= Tolerance &&
               FMath::Abs(z) <= Tolerance;
    }
    static const FVector ZeroVector;
    static const FVector OneVector;
    static const FVector UpVector;
    static const FVector ForwardVector;
    static const FVector RightVector;
};
inline FVector FVector::operator+(const FVector& Other) const
{
    return { x + Other.x, y + Other.y, z + Other.z };
}

inline FVector& FVector::operator+=(const FVector& Other)
{
    x += Other.x; y += Other.y; z += Other.z;
    return *this;
}

inline FVector FVector::operator-(const FVector& Other) const
{
    return { x - Other.x, y - Other.y, z - Other.z };
}

inline FVector& FVector::operator-=(const FVector& Other)
{
    x -= Other.x; y -= Other.y; z -= Other.z;
    return *this;
}

inline FVector FVector::operator*(const FVector& Other) const
{
    return { x * Other.x, y * Other.y, z * Other.z };
}

inline FVector FVector::operator*(float Scalar) const
{
    return { x * Scalar, y * Scalar, z * Scalar };
}

inline FVector& FVector::operator*=(float Scalar)
{
    x *= Scalar; y *= Scalar; z *= Scalar;
    return *this;
}

inline FVector FVector::operator/(const FVector& Other) const
{
    return { x / Other.x, y / Other.y, z / Other.z };
}

inline FVector FVector::operator/(float Scalar) const
{
    return { x / Scalar, y / Scalar, z / Scalar };
}

inline FVector& FVector::operator/=(float Scalar)
{
    x /= Scalar; y /= Scalar; z /= Scalar;
    return *this;
}

inline FVector FVector::operator-() const
{
    return { -x, -y, -z };
}