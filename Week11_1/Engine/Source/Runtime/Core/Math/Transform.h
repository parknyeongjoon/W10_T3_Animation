#pragma once
#include "Quat.h"
#include "Vector.h"
#include "Rotator.h"

class FTransform
{
public:
    FTransform() : Rotation(FQuat::Identity), Location(FVector::ZeroVector), Scale(FVector::OneVector) {}
    FTransform(FQuat InRotation, FVector InLocation, FVector InScale) : Rotation(InRotation), Location(InLocation), Scale(InScale) {}
    FTransform(const FMatrix& InMatrix);

    FQuat Rotation;
    FVector Location;
    FVector Scale;

    FVector GetLocation() const { return Location; }
    FQuat GetRotation() const { return Rotation; }
    FVector GetScale() const { return Scale; }

    void SetRotation(const FQuat& InRotation) { Rotation = InRotation; }
    void SetRotation(const FRotator& InRotation) { Rotation = InRotation.ToQuaternion(); }
    void SetLocation(const FVector& InLocation) { Location = InLocation; }
    void SetScale(const FVector& InScale) { Scale = InScale; }
    void SetFromMatrix(const FMatrix& InMatrix);

    FRotator Rotator() const
    {
        return FRotator(Rotation);
    }

    const static FTransform Identity;

    static FTransform Blend(const FTransform& Atom1, const FTransform& Atom2, float Alpha);

    // 4x4 Matrix로 변환하는 함수
    FMatrix ToMatrixWithScale() const;
    FMatrix ToMatrixNoScale() const;

    inline friend FArchive& operator<<(FArchive& Ar, FTransform& Transform)
    {
        return Ar << Transform.Location
            << Transform.Rotation
            << Transform.Scale;
    }

};

inline const FTransform FTransform::Identity = FTransform();

inline FTransform::FTransform(const FMatrix& InMatrix)
{
    SetFromMatrix(InMatrix);
}

inline void FTransform::SetFromMatrix(const FMatrix& InMatrix)
{
    // 행렬 복사본 생성
    FMatrix M = InMatrix;

    // 스케일 추출
    Scale = M.ExtractScaling();

    // 음수 스케일링 처리
    if (InMatrix.Determinant() < 0.f)
    {
        // 음수 행렬식은 음수 스케일이 있다는 의미입니다.
        // X축을 따라 음수 스케일이 있다고 가정하고 변환을 수정합니다.
        // 어떤 축을 선택하든 '외관'은 동일합니다.
        Scale.X = -Scale.X;

        // X축 방향 반전
        M.SetAxis(0, -M.GetScaledAxis(EAxis::X));
    }

    // 스케일이 제거된 행렬에서 회전값 추출
    Rotation = FQuat(M);
    Rotation.Normalize();

    // 이동값 추출
    Location = InMatrix.GetOrigin();
}

inline FTransform FTransform::Blend(const FTransform& Atom1, const FTransform& Atom2, float Alpha)
{
    FTransform Result;

    Result.Rotation = FQuat::Slerp(Atom1.Rotation, Atom2.Rotation, Alpha);
    Result.Location = FMath::Lerp(Atom1.Location, Atom2.Location, Alpha);
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