#pragma once
#include "Container/String.h"
#include "MathUtility.h"

struct FVector;
struct FQuat;
struct FMatrix;

// 회전 정보를 Degree 단위로 저장하는 구조체
struct FRotator
{
    /** Rotation around the right axis (around Y axis), Looking up and down (0=Straight Ahead, +Up, -Down) */
    float Pitch;
    /** Rotation around the up axis (around Z axis), Turning around (0=Forward, +Right, -Left)*/
    float Yaw;
    /** Rotation around the forward axis (around X axis), Tilting your head, (0=Straight, +Clockwise, -CCW) */
    float Roll;

    explicit FRotator()
        : Pitch(0.0f), Yaw(0.0f), Roll(0.0f)
    {}

    explicit FRotator(float InPitch, float InYaw, float InRoll)
        : Pitch(InPitch), Yaw(InYaw), Roll(InRoll)
    {}

    FRotator(const FRotator& Other)
        : Pitch(Other.Pitch), Yaw(Other.Yaw), Roll(Other.Roll)
    {}

    explicit FRotator(const FVector& InVector);
    explicit FRotator(const FQuat& InQuat);

    void Serialize(FArchive& Ar) const
    {
        Ar << Pitch << Yaw << Roll;
    }

    void Deserialize(FArchive& Ar)
    {
        Ar >> Pitch >> Yaw >> Roll;
    }

    FRotator operator+(const FRotator& Other) const;
    FRotator& operator+=(const FRotator& Other);

    FRotator operator-(const FRotator& Other) const;
    FRotator& operator-=(const FRotator& Other);

    FRotator operator*(float Scalar) const;
    FRotator& operator*=(float Scalar);

    FRotator operator/(const FRotator& Other) const;
    FRotator operator/(float Scalar) const;
    FRotator& operator/=(float Scalar);

    FRotator operator-() const;

    bool operator==(const FRotator& Other) const;
    bool operator!=(const FRotator& Other) const;

    bool IsNearlyZero(float Tolerance = KINDA_SMALL_NUMBER) const;
    bool IsZero() const;

    bool Equals(const FRotator& Other, float Tolerance = KINDA_SMALL_NUMBER) const;

    FRotator Add(float DeltaPitch, float DeltaYaw, float DeltaRoll) const;

    FRotator FromQuaternion(const FQuat& InQuat) const;
    FQuat ToQuaternion() const;
    FVector ToVector() const;
    FMatrix ToMatrix() const;

    FRotator GetNormalized() const;
    void Normalize();
    
    FVector RotateVector(const FVector& Vec) const;
    
    static float ClampAxis(float Angle);
    static float NormalizeAxis(float Angle);

    FVector GetForwardVector() const;
    FVector GetRightVector() const;
    FVector GetUpVector() const;

    FVector Vector() const;
    
    static const FRotator ZeroRotator;
};