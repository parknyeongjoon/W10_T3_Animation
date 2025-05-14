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

    // 4x4 Matrix로 변환하는 함수
    FMatrix ToMatrixWithScale() const;
    FMatrix ToMatrixNoScale() const;
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

// DirectX 스타일: Translation * Rotation * Scale 순서 (right-to-left)
inline FMatrix FTransform::ToMatrixWithScale() const
{
    FMatrix M;
    
    // Quaternion을 회전 행렬로 변환 (열 우선)
    float x2 = Rotation.X * Rotation.X;
    float y2 = Rotation.Y * Rotation.Y;
    float z2 = Rotation.Z * Rotation.Z;
    float xy = Rotation.X * Rotation.Y;
    float xz = Rotation.X * Rotation.Z;
    float yz = Rotation.Y * Rotation.Z;
    float wx = Rotation.W * Rotation.X;
    float wy = Rotation.W * Rotation.Y;
    float wz = Rotation.W * Rotation.Z;
    
    // 첫 번째 열 (X축)
    M.M[0][0] = (1.0f - 2.0f * (y2 + z2)) * Scale.X;
    M.M[1][0] = (2.0f * (xy + wz)) * Scale.X;
    M.M[2][0] = (2.0f * (xz - wy)) * Scale.X;
    M.M[3][0] = 0.0f;
    
    // 두 번째 열 (Y축)
    M.M[0][1] = (2.0f * (xy - wz)) * Scale.Y;
    M.M[1][1] = (1.0f - 2.0f * (x2 + z2)) * Scale.Y;
    M.M[2][1] = (2.0f * (yz + wx)) * Scale.Y;
    M.M[3][1] = 0.0f;
    
    // 세 번째 열 (Z축)
    M.M[0][2] = (2.0f * (xz + wy)) * Scale.Z;
    M.M[1][2] = (2.0f * (yz - wx)) * Scale.Z;
    M.M[2][2] = (1.0f - 2.0f * (x2 + y2)) * Scale.Z;
    M.M[3][2] = 0.0f;
    
    // 네 번째 열 (위치)
    M.M[0][3] = Location.X;
    M.M[1][3] = Location.Y;
    M.M[2][3] = Location.Z;
    M.M[3][3] = 1.0f;
    
    return M;
}

// Scale 없는 버전
inline FMatrix FTransform::ToMatrixNoScale() const
{
    FMatrix M;
    
    // Quaternion to matrix conversion
    float x2 = Rotation.X * Rotation.X;
    float y2 = Rotation.Y * Rotation.Y;
    float z2 = Rotation.Z * Rotation.Z;
    float xy = Rotation.X * Rotation.Y;
    float xz = Rotation.X * Rotation.Z;
    float yz = Rotation.Y * Rotation.Z;
    float wx = Rotation.W * Rotation.X;
    float wy = Rotation.W * Rotation.Y;
    float wz = Rotation.W * Rotation.Z;
    
    // 첫 번째 열 (X축)
    M.M[0][0] = 1.0f - 2.0f * (y2 + z2);
    M.M[1][0] = 2.0f * (xy + wz);
    M.M[2][0] = 2.0f * (xz - wy);
    M.M[3][0] = 0.0f;
    
    // 두 번째 열 (Y축)
    M.M[0][1] = 2.0f * (xy - wz);
    M.M[1][1] = 1.0f - 2.0f * (x2 + z2);
    M.M[2][1] = 2.0f * (yz + wx);
    M.M[3][1] = 0.0f;
    
    // 세 번째 열 (Z축)
    M.M[0][2] = 2.0f * (xz + wy);
    M.M[1][2] = 2.0f * (yz - wx);
    M.M[2][2] = 1.0f - 2.0f * (x2 + y2);
    M.M[3][2] = 0.0f;
    
    // 네 번째 열 (위치)
    M.M[0][3] = Location.X;
    M.M[1][3] = Location.Y;
    M.M[2][3] = Location.Z;
    M.M[3][3] = 1.0f;
    
    return M;
}