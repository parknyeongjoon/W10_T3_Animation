#include "Rotator.h"

#include "JungleMath.h"
#include "Vector.h"
#include "Quat.h"
#include "Matrix.h"
#include "Misc/Parse.h"

const FRotator FRotator::ZeroRotator = FRotator(0.0f, 0.0f, 0.0f);

FRotator::FRotator(const FVector& InVector)
    : Pitch(FMath::RadiansToDegrees(InVector.Y)), Yaw(FMath::RadiansToDegrees(InVector.Z)), Roll(FMath::RadiansToDegrees(InVector.X))
{
}

FRotator::FRotator(const FQuat& InQuat)
{
    *this = InQuat.Rotator();
}

FRotator FRotator::operator+(const FRotator& Other) const
{
    return FRotator(Pitch + Other.Pitch, Yaw + Other.Yaw, Roll + Other.Roll);
}

FRotator& FRotator::operator+=(const FRotator& Other)
{
    Pitch += Other.Pitch; Yaw += Other.Yaw; Roll += Other.Roll;
    return *this;
}

FRotator FRotator::operator-(const FRotator& Other) const
{
    return FRotator{ Pitch - Other.Pitch, Yaw - Other.Yaw, Roll - Other.Roll };
}

FRotator& FRotator::operator-=(const FRotator& Other)
{
    Pitch -= Other.Pitch; Yaw -= Other.Yaw; Roll -= Other.Roll;
    return *this;
}

FRotator FRotator::operator*(float Scalar) const
{
    return FRotator{ Pitch * Scalar, Yaw * Scalar, Roll * Scalar };
}

FRotator& FRotator::operator*=(float Scalar)
{
    Pitch *= Scalar; Yaw *= Scalar; Roll *= Scalar;
    return *this;
}

FRotator FRotator::operator/(const FRotator& Other) const
{
    return FRotator{ Pitch / Other.Pitch, Yaw / Other.Yaw, Roll / Other.Roll };
}

FRotator FRotator::operator/(float Scalar) const
{
    return FRotator{ Pitch / Scalar, Yaw / Scalar, Roll / Scalar };
}

FRotator& FRotator::operator/=(float Scalar)
{
    Pitch /= Scalar; Yaw /= Scalar; Roll /= Scalar;
    return *this;
}

FRotator FRotator::operator-() const
{
    return FRotator{ -Pitch, -Yaw, -Roll };
}

bool FRotator::operator==(const FRotator& Other) const
{
    return Pitch == Other.Pitch && Yaw == Other.Yaw && Roll == Other.Roll;
}

bool FRotator::operator!=(const FRotator& Other) const
{
    return Pitch != Other.Pitch || Yaw != Other.Yaw || Roll != Other.Roll;
}

bool FRotator::IsNearlyZero(float Tolerance) const
{
    return FMath::Abs(Pitch) <= Tolerance && FMath::Abs(Yaw) <= Tolerance && FMath::Abs(Roll) <= Tolerance;
}

bool FRotator::IsZero() const
{
    return Pitch == 0.0f && Yaw == 0.0f && Roll == 0.0f;
}

bool FRotator::Equals(const FRotator& Other, float Tolerance) const
{
    return FMath::Abs(Pitch - Other.Pitch) <= Tolerance && FMath::Abs(Yaw - Other.Yaw) <= Tolerance && FMath::Abs(Roll - Other.Roll) <= Tolerance;

}

FRotator FRotator::Add(float DeltaPitch, float DeltaYaw, float DeltaRoll) const
{
    return FRotator(Pitch + DeltaPitch, Yaw + DeltaYaw, Roll + DeltaRoll);
}

FRotator FRotator::FromQuaternion(const FQuat& InQuat) const
{
    return FRotator(InQuat);
}

FQuat FRotator::ToQuaternion() const
{
    float DegToRad = PI / 180.0f;
    float Div = DegToRad / 2.0f;
    float SP, SY, SR;
    float CP, CY, CR;

    const float PitchNoWinding = FMath::Fmod(Pitch, 360.0f);
    const float YawNoWinding = FMath::Fmod(Yaw, 360.0f);
    const float RollNoWinding = FMath::Fmod(Roll, 360.0f);

    FMath::SinCos(&SP, &CP, PitchNoWinding * Div);
    FMath::SinCos(&SY, &CY, YawNoWinding * Div);
    FMath::SinCos(&SR, &CR, RollNoWinding * Div);
	
    FQuat RotationQuat;
    RotationQuat.X = CR * SP * SY - SR * CP * CY;
    RotationQuat.Y = -CR * SP * CY - SR * CP * SY;
    RotationQuat.Z = CR * CP * SY - SR * SP * CY;
    RotationQuat.W = CR * CP * CY + SR * SP * SY;

    return RotationQuat;
}

FVector FRotator::ToVector() const
{
    const float PitchNoWinding = FMath::Fmod(Pitch, 360.f);
    const float YawNoWinding = FMath::Fmod(Yaw, 360.f);

    float CP, SP, CY, SY;
    FMath::SinCos( &SP, &CP, FMath::DegreesToRadians(PitchNoWinding) );
    FMath::SinCos( &SY, &CY, FMath::DegreesToRadians(YawNoWinding) );
    FVector V = FVector( CP*CY, CP*SY, SP );

    if (!_finite(V.X) || !_finite(V.Y) || !_finite(V.Z))
    {
        V = FVector::ForwardVector;
    }

    return V;
}

FVector FRotator::RotateVector(const FVector& Vec) const
{
    return ToQuaternion().RotateVector(Vec);
}

FMatrix FRotator::ToMatrix() const
{
    return FMatrix::GetRotationMatrix(*this);
}

float FRotator::ClampAxis(float Angle)
{
    Angle = FMath::Fmod(Angle, 360.0f);
    if (Angle < 0.0f)
    {
        Angle += 360.0f;
    }
    return Angle;
}

FRotator FRotator::GetNormalized() const
{
    return FRotator{ FMath::UnwindDegrees(Pitch), FMath::UnwindDegrees(Yaw), FMath::UnwindDegrees(Roll) };
}

void FRotator::Normalize()
{
    Pitch = FMath::UnwindDegrees(Pitch);
    Yaw = FMath::UnwindDegrees(Yaw);
    Roll = FMath::UnwindDegrees(Roll);
}

float FRotator::NormalizeAxis(float Angle)
{
    Angle = ClampAxis(Angle);

    if (Angle > 180.0f)
    {
        // shift to (-180,180]
        Angle -= 360.0f;
    }

    return Angle;
}

FVector FRotator::GetForwardVector() const
{
    FVector Forward = FVector::ForwardVector;
    Forward = JungleMath::FVectorRotate(Forward, *this);
    return Forward;
}

FVector FRotator::GetRightVector() const
{
    FVector Right = FVector::RightVector;
    Right = JungleMath::FVectorRotate(Right, *this);
    return Right;
}

FVector FRotator::GetUpVector() const
{
    FVector Up = FVector::UpVector;
    Up = JungleMath::FVectorRotate(Up, *this);
    return Up;
}

FVector FRotator::Vector() const
{
    // Extremely large but valid values (or invalid values from uninitialized vars) can cause SinCos to return NaN/Inf, so catch that here. Similar to what is done in FRotator::Quaternion().
    // if (FMath::Abs(Pitch) > UE_FLOAT_NON_FRACTIONAL ||
    //     FMath::Abs(Yaw  ) > UE_FLOAT_NON_FRACTIONAL ||
    //     FMath::Abs(Roll ) > UE_FLOAT_NON_FRACTIONAL)
    // {
    //     logOrEnsureNanError(TEXT("FRotator::Vector() provided with unreasonably large input values (%s), possible use of uninitialized variable?"), *ToString());
    // }
	
    // Remove winding and clamp to [-360, 360]
    const float PitchNoWinding = FMath::Fmod(Pitch, (float)360.0);
    const float YawNoWinding = FMath::Fmod(Yaw, (float)360.0);

    float CP, SP, CY, SY;
    FMath::SinCos( &SP, &CP, FMath::DegreesToRadians(PitchNoWinding) );
    FMath::SinCos( &SY, &CY, FMath::DegreesToRadians(YawNoWinding) );
    FVector V = FVector( CP*CY, CP*SY, SP );

    // Error checking
    // if (V.ContainsNaN())
    // {
    //     UE_LOG(LogLevel::Error, TEXT("FRotator::Vector() resulted in NaN/Inf with input: %s output: %s"), *ToString(), *V.ToString());
    //     V = FVector::ForwardVector;
    // }

    return V;
}
