#include "Quat.h"

#include "Vector.h"
#include "Matrix.h"
#include "Rotator.h"

FQuat::FQuat(const FVector& Axis, float Angle)
{
    float HalfAngle = Angle * 0.5f;
    float SinHalfAngle = sinf(HalfAngle);
    float CosHalfAngle = cosf(HalfAngle);

    X = Axis.X * SinHalfAngle;
    Y = Axis.Y * SinHalfAngle;
    Z = Axis.Z * SinHalfAngle;
    W = CosHalfAngle;
}

FQuat::FQuat(const FMatrix& InMatrix)
{
    float S;
    // Check diagonal (trace)
    const float trace = InMatrix.M[0][0] + InMatrix.M[1][1] + InMatrix.M[2][2]; // 행렬의 Trace 값 (대각합)

    if (trace > 0.0f) 
    {
        float InvS = FMath::InvSqrt(trace + 1.f);
        this->W = 0.5f * (1.f / InvS);
        S = 0.5f * InvS;

        this->X = ((InMatrix.M[1][2] - InMatrix.M[2][1]) * S);
        this->Y = ((InMatrix.M[2][0] - InMatrix.M[0][2]) * S);
        this->Z = ((InMatrix.M[0][1] - InMatrix.M[1][0]) * S);
    } 
    else 
    {
        // diagonal is negative
        int32 i = 0;

        if (InMatrix.M[1][1] > InMatrix.M[0][0])
            i = 1;

        if (InMatrix.M[2][2] > InMatrix.M[i][i])
            i = 2;

        static constexpr int32 nxt[3] = { 1, 2, 0 };
        const int32 j = nxt[i];
        const int32 k = nxt[j];
 
        S = InMatrix.M[i][i] - InMatrix.M[j][j] - InMatrix.M[k][k] + 1.0f;

        float InvS = FMath::InvSqrt(S);

        float qt[4];
        qt[i] = 0.5f * (1.f / InvS);

        S = 0.5f * InvS;

        qt[3] = (InMatrix.M[j][k] - InMatrix.M[k][j]) * S;
        qt[j] = (InMatrix.M[i][j] + InMatrix.M[j][i]) * S;
        qt[k] = (InMatrix.M[i][k] + InMatrix.M[k][i]) * S;

        this->X = qt[0];
        this->Y = qt[1];
        this->Z = qt[2];
        this->W = qt[3];

    }
}

FQuat FQuat::operator*(const FQuat& Other) const
{
    return FQuat(
            W * Other.W - X * Other.X - Y * Other.Y - Z * Other.Z,
            W * Other.X + X * Other.W + Y * Other.Z - Z * Other.Y,
            W * Other.Y - X * Other.Z + Y * Other.W + Z * Other.X,
            W * Other.Z + X * Other.Y - Y * Other.X + Z * Other.W
        );
}

FVector FQuat::RotateVector(const FVector& Vec) const
{
    // 벡터를 쿼터니언으로 변환
    FQuat vecQuat(0.0f, Vec.X, Vec.Y, Vec.Z);
    // 회전 적용 (q * vec * q^-1)
    FQuat conjugate = FQuat(W, -X, -Y, -Z); // 쿼터니언의 켤레
    FQuat result = *this * vecQuat * conjugate;

    return FVector(result.X, result.Y, result.Z); // 회전된 벡터 반환
}

bool FQuat::IsNormalized() const
{
    return fabs(W * W + X * X + Y * Y + Z * Z - 1.0f) < 1e-6f;
}

void FQuat::Normalize(float Tolerance)
{
    float Magnitude = sqrtf(W * W + X * X + Y * Y + Z * Z);
    W /= Magnitude;
    X /= Magnitude;
    Y /= Magnitude;
    Z /= Magnitude;
}

FQuat FQuat::GetUnsafeNormal() const
{
    float Magnitude = sqrtf(W * W + X * X + Y * Y + Z * Z);
    return FQuat(W / Magnitude, X / Magnitude, Y / Magnitude, Z / Magnitude);
}

FQuat FQuat::GetSafeNormal(float Tolerance) const
{
    float Magnitude = sqrtf(W * W + X * X + Y * Y + Z * Z);
    if (Magnitude < Tolerance)
    {
        return FQuat(1, 0, 0, 0);
    }
    return FQuat(W / Magnitude, X / Magnitude, Y / Magnitude, Z / Magnitude);
}

FQuat FQuat::FromAxisAngle(const FVector& Axis, float Angle)
{
    float halfAngle = Angle * 0.5f;
    float sinHalfAngle = sinf(halfAngle);
    return FQuat(cosf(halfAngle), Axis.X * sinHalfAngle, Axis.Y * sinHalfAngle, Axis.Z * sinHalfAngle);
}

FQuat FQuat::CreateRotation(float roll, float pitch, float yaw)
{
    // 각도를 라디안으로 변환
    float radRoll = roll * (PI / 180.0f);
    float radPitch = pitch * (PI / 180.0f);
    float radYaw = yaw * (PI / 180.0f);

    // 각 축에 대한 회전 쿼터니언 계산
    FQuat qRoll = FQuat(FVector(1.0f, 0.0f, 0.0f), radRoll); // X축 회전
    FQuat qPitch = FQuat(FVector(0.0f, 1.0f, 0.0f), radPitch); // Y축 회전
    FQuat qYaw = FQuat(FVector(0.0f, 0.0f, 1.0f), radYaw); // Z축 회전

    // 회전 순서대로 쿼터니언 결합 (Y -> X -> Z)
    return qRoll * qPitch * qYaw;
}

FMatrix FQuat::ToMatrix() const
{
    FMatrix R;
    const float x2 = X + X;    const float y2 = Y + Y;    const float z2 = Z + Z;
    const float xx = X * x2;   const float xy = X * y2;   const float xz = X * z2;
    const float yy = Y * y2;   const float yz = Y * z2;   const float zz = Z * z2;
    const float wx = W * x2;   const float wy = W * y2;   const float wz = W * z2;

    R.M[0][0] = 1.0f - (yy + zz);	R.M[1][0] = xy - wz;				R.M[2][0] = xz + wy;			R.M[3][0] = 0.0f;
    R.M[0][1] = xy + wz;			R.M[1][1] = 1.0f - (xx + zz);		R.M[2][1] = yz - wx;			R.M[3][1] = 0.0f;
    R.M[0][2] = xz - wy;			R.M[1][2] = yz + wx;				R.M[2][2] = 1.0f - (xx + yy);	R.M[3][2] = 0.0f;
    R.M[0][3] = 0.0f;				R.M[1][3] = 0.0f;					R.M[2][3] = 0.0f;

    return R;
}

bool FQuat::Equals(const FQuat& Q, float Tolerance) const
{
    return (FMath::Abs(X - Q.X) <= Tolerance && FMath::Abs(Y - Q.Y) <= Tolerance && FMath::Abs(Z - Q.Z) <= Tolerance && FMath::Abs(W - Q.W) <= Tolerance)
        || (FMath::Abs(X + Q.X) <= Tolerance && FMath::Abs(Y + Q.Y) <= Tolerance && FMath::Abs(Z + Q.Z) <= Tolerance && FMath::Abs(W + Q.W) <= Tolerance);
}

FRotator FQuat::Rotator() const
{
    const float SingularityTest = Z * X - W * Y;
    const float YawY = 2.f * (W * Z + X * Y);
    const float YawX = (1.f - 2.f * (FMath::Square(Y) + FMath::Square(Z)));

    // reference 
    // http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
    // http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/

    // this value was found from experience, the above websites recommend different values
    // but that isn't the case for us, so I went through different testing, and finally found the case 
    // where both of world lives happily. 
    const float SINGULARITY_THRESHOLD = 0.4999995f;
    const float RAD_TO_DEG = (180.f / PI);
    float Pitch, Yaw, Roll;

    if (SingularityTest < -SINGULARITY_THRESHOLD)
    {
        Pitch = -90.f;
        Yaw = (FMath::Atan2(YawY, YawX) * RAD_TO_DEG);
        Roll = FRotator::NormalizeAxis(-Yaw - (2.f * FMath::Atan2(X, W) * RAD_TO_DEG));
    }
    else if (SingularityTest > SINGULARITY_THRESHOLD)
    {
        Pitch = 90.f;
        Yaw = (FMath::Atan2(YawY, YawX) * RAD_TO_DEG);
        Roll = FRotator::NormalizeAxis(Yaw - (2.f * FMath::Atan2(X, W) * RAD_TO_DEG));
    }
    else
    {
        Pitch = (FMath::Asin(2.f * SingularityTest) * RAD_TO_DEG);
        Yaw = (FMath::Atan2(YawY, YawX) * RAD_TO_DEG);
        Roll = (FMath::Atan2(-2.f * (W*X + Y*Z), (1.f - 2.f * (FMath::Square(X) + FMath::Square(Y)))) * RAD_TO_DEG);
    }

    FRotator RotatorFromQuat = FRotator(Pitch, Yaw, Roll);

    return RotatorFromQuat;
}

float FQuat::AngularDistance(const FQuat& Q) const
{
    float InnerProd = X*Q.X + Y*Q.Y + Z*Q.Z + W*Q.W;
    return FMath::Acos((2 * InnerProd * InnerProd) - 1.f);
}

FQuat FQuat::Slerp(const FQuat& Quat1, const FQuat& Quat2, float Slerp)
{
    return Slerp_NotNormalized(Quat1, Quat2, Slerp).GetSafeNormal();
}

FQuat FQuat::Slerp_NotNormalized(const FQuat& Quat1, const FQuat& Quat2, float Slerp)
{
    // Get cosine of angle between quats.
    float RawCosom =
        Quat1.X * Quat2.X +
        Quat1.Y * Quat2.Y +
        Quat1.Z * Quat2.Z +
        Quat1.W * Quat2.W;

    // Unaligned quats - compensate, results in taking shorter route.
        
    const float Sign = (RawCosom >= 0.0f) ? static_cast<float>(1.0) : static_cast<float>(-1.0);
    RawCosom *= Sign;
		
    float Scale0 = static_cast<float>(1.0) - Slerp;
    float Scale1 = Slerp * Sign;
		
    if (RawCosom < static_cast<float>(0.9999))
    {
        const float Omega = FMath::Acos(RawCosom);
        const float InvSin = static_cast<float>(1.0) / FMath::Sin(Omega);
        Scale0 = FMath::Sin(Scale0 * Omega) * InvSin;
        Scale1 = FMath::Sin(Scale1 * Omega) * InvSin;
    }
		
    return FQuat(
        Scale0 * Quat1.W + Scale1 * Quat2.W,
        Scale0 * Quat1.X + Scale1 * Quat2.X,
        Scale0 * Quat1.Y + Scale1 * Quat2.Y,
        Scale0 * Quat1.Z + Scale1 * Quat2.Z);
}
