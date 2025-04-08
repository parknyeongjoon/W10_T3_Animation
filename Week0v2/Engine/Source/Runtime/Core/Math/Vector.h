#pragma once

#include <DirectXMath.h>

#include <cmath>

#include "MathUtility.h"

struct FMath;

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
};

// 3D 벡터
struct FVector
{
    float x, y, z;
    FVector(float _x = 0, float _y = 0, float _z = 0) : x(_x), y(_y), z(_z) {}

    FVector operator-(const FVector& other) const {
        return FVector(x - other.x, y - other.y, z - other.z);
    }
    FVector operator+(const FVector& other) const {
        return FVector(x + other.x, y + other.y, z + other.z);
    }

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
    // 스칼라 곱셈
    FVector operator*(float scalar) const {
        return FVector(x * scalar, y * scalar, z * scalar);
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

    static const FVector ZeroVector;
    static const FVector OneVector;
    static const FVector UpVector;
    static const FVector ForwardVector;
    static const FVector RightVector;
};
