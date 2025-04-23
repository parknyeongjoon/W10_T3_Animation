#include "Engine/Source/Runtime/Core/Math/JungleMath.h"
#include <DirectXMath.h>

#include "MathUtility.h"
#include "Rotator.h"

using namespace DirectX;
FVector4 JungleMath::ConvertV3ToV4(FVector vec3)
{
	FVector4 newVec4;
	newVec4.x = vec3.x;
	newVec4.y = vec3.y;
	newVec4.z = vec3.z;
	return newVec4;
}



FMatrix JungleMath::CreateModelMatrix(FVector translation, FVector rotation, FVector scale)
{
    FMatrix Translation = FMatrix::CreateTranslationMatrix(translation);

    FMatrix Rotation = FMatrix::CreateRotationMatrix(rotation.x, rotation.y, rotation.z);
    //FMatrix Rotation = JungleMath::EulerToQuaternion(rotation).ToMatrix();

    FMatrix Scale = FMatrix::CreateScaleMatrix(scale.x, scale.y, scale.z);
    return Scale * Rotation * Translation;
}

FMatrix JungleMath::CreateModelMatrix(FVector translation, FQuat rotation, FVector scale)
{
    FMatrix Translation = FMatrix::CreateTranslationMatrix(translation);
    FMatrix Rotation = rotation.ToMatrix();
    FMatrix Scale = FMatrix::CreateScaleMatrix(scale.x, scale.y, scale.z);
    return Scale * Rotation * Translation;
}
FMatrix JungleMath::CreateViewMatrix(FVector eye, FVector target, FVector up)
{
    FVector zAxis = (target - eye).Normalize();  // DirectX는 LH이므로 -z가 아니라 +z 사용
    FVector xAxis = (up.Cross(zAxis)).Normalize();
    FVector yAxis = zAxis.Cross(xAxis);

    FMatrix View;
    View.M[0][0] = xAxis.x; View.M[0][1] = yAxis.x; View.M[0][2] = zAxis.x; View.M[0][3] = 0;
    View.M[1][0] = xAxis.y; View.M[1][1] = yAxis.y; View.M[1][2] = zAxis.y; View.M[1][3] = 0;
    View.M[2][0] = xAxis.z; View.M[2][1] = yAxis.z; View.M[2][2] = zAxis.z; View.M[2][3] = 0;
    View.M[3][0] = -xAxis.Dot(eye);
    View.M[3][1] = -yAxis.Dot(eye);
    View.M[3][2] = -zAxis.Dot(eye);
    View.M[3][3] = 1;

    return View;
}

FMatrix JungleMath::CreateProjectionMatrix(float fov, float aspect, float nearPlane, float farPlane)
{
    float tanHalfFOV = tan(fov / 2.0f);
    float depth = farPlane - nearPlane;

    FMatrix Projection = {};
    Projection.M[0][0] = 1.0f / (aspect * tanHalfFOV);
    Projection.M[1][1] = 1.0f / tanHalfFOV;
    Projection.M[2][2] = farPlane / depth;
    Projection.M[2][3] = 1.0f;
    Projection.M[3][2] = -(nearPlane * farPlane) / depth;
    Projection.M[3][3] = 0.0f;  

    return Projection;
}

FMatrix JungleMath::CreateOrthoProjectionMatrix(float width, float height, float nearPlane, float farPlane)
{
    float r = width * 0.5f;
    float t = height * 0.5f;
    float invDepth = 1.0f / (farPlane - nearPlane);

    FMatrix Projection = {};
    Projection.M[0][0] = 1.0f / r;
    Projection.M[1][1] = 1.0f / t;
    Projection.M[2][2] = invDepth;
    Projection.M[3][2] = -nearPlane * invDepth;
    Projection.M[3][3] = 1.0f;

    return Projection;
}

FMatrix JungleMath::CreateOrthoProjectionMatrix(float left, float right, float bottom, float top, float nearZ, float farZ)
{
    // 직교 투영 행렬 구성 (행 우선 방식)
    // DirectX 스타일 깊이 범위(0~1)를 사용한다고 가정합니다
    
    float width = right - left;
    float height = top - bottom;
    float depth = farZ - nearZ;
    
    // 유효하지 않은 입력값 검사
    if (width < 1e-6f || height < 1e-6f || depth < 1e-6f)
    {
        // 오류 방지를 위한 기본값 반환
        return FMatrix::Identity;
    }
    
    FMatrix result;
    
    result.M[0][0] = 2.0f / width;
    result.M[0][1] = 0.0f;
    result.M[0][2] = 0.0f;
    result.M[0][3] = 0.0f;
    
    result.M[1][0] = 0.0f;
    result.M[1][1] = 2.0f / height;
    result.M[1][2] = 0.0f;
    result.M[1][3] = 0.0f;
    
    result.M[2][0] = 0.0f;
    result.M[2][1] = 0.0f;
    result.M[2][2] = 1.0f / depth;
    result.M[2][3] = 0.0f;
    
    result.M[3][0] = -(right + left) / width;
    result.M[3][1] = -(top + bottom) / height;
    result.M[3][2] = -nearZ / depth;
    result.M[3][3] = 1.0f;
    
    return result;
}

FVector JungleMath::FVectorRotate(FVector& origin, const FVector& rotation)
{
    FQuat quaternion = JungleMath::EulerToQuaternion(rotation);
    // 쿼터니언을 이용해 벡터 회전 적용
    return quaternion.RotateVector(origin);
}
FQuat JungleMath::EulerToQuaternion(const FVector& eulerDegrees)
{
    float yaw = DegToRad(eulerDegrees.z);   // Z축 Yaw
    float pitch = DegToRad(eulerDegrees.y); // Y축 Pitch
    float roll = DegToRad(eulerDegrees.x);  // X축 Roll

    float halfYaw = yaw * 0.5f;
    float halfPitch = pitch * 0.5f;
    float halfRoll = roll * 0.5f;

    float cosYaw = cos(halfYaw);
    float sinYaw = sin(halfYaw);
    float cosPitch = cos(halfPitch);
    float sinPitch = sin(halfPitch);
    float cosRoll = cos(halfRoll);
    float sinRoll = sin(halfRoll);

    FQuat quat;
    quat.w = cosYaw * cosPitch * cosRoll + sinYaw * sinPitch * sinRoll;
    quat.x = cosYaw * cosPitch * sinRoll - sinYaw * sinPitch * cosRoll;
    quat.y = cosYaw * sinPitch * cosRoll + sinYaw * cosPitch * sinRoll;
    quat.z = sinYaw * cosPitch * cosRoll - cosYaw * sinPitch * sinRoll;

    quat.Normalize();
    return quat;
}
FVector JungleMath::QuaternionToEuler(const FQuat& quat)
{
    FVector euler;

    // 쿼터니언 정규화
    FQuat q = quat;
    q.Normalize();

    // Yaw (Z 축 회전)
    float sinYaw = 2.0f * (q.w * q.z + q.x * q.y);
    float cosYaw = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
    euler.z = RadToDeg(atan2(sinYaw, cosYaw));

    // Pitch (Y 축 회전, 짐벌락 방지)
    float sinPitch = 2.0f * (q.w * q.y - q.z * q.x);
    if (fabs(sinPitch) >= 1.0f)
    {
        euler.y = RadToDeg(static_cast<float>(copysign(PI / 2, sinPitch))); // 🔥 Gimbal Lock 방지
    }
    else
    {
        euler.y = RadToDeg(asin(sinPitch));
    }

    // Roll (X 축 회전)
    float sinRoll = 2.0f * (q.w * q.x + q.y * q.z);
    float cosRoll = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
    euler.x = RadToDeg(atan2(sinRoll, cosRoll));
    return euler;
}
FVector JungleMath::FVectorRotate(FVector& origin, const FRotator& InRotation)
{
    return InRotation.ToQuaternion().RotateVector(origin);
}

FMatrix JungleMath::CreateRotationMatrix(FVector rotation)
{
    XMVECTOR quatX = XMQuaternionRotationAxis(XMVectorSet(1, 0, 0, 0), DegToRad(rotation.x));
    XMVECTOR quatY = XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), DegToRad(rotation.y));
    XMVECTOR quatZ = XMQuaternionRotationAxis(XMVectorSet(0, 0, 1, 0), DegToRad(rotation.z));

    XMVECTOR rotationQuat = XMQuaternionMultiply(quatZ, XMQuaternionMultiply(quatY, quatX));
    rotationQuat = XMQuaternionNormalize(rotationQuat);  // 정규화 필수

    XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(rotationQuat);
    FMatrix result = FMatrix::Identity;  // 기본값 설정 (단위 행렬)

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            result.M[i][j] = rotationMatrix.r[i].m128_f32[j];  // XMMATRIX에서 FMatrix로 값 복사
        }
    }
    return result;
}


float JungleMath::RadToDeg(float radian)
{
    return static_cast<float>(radian * (180.0f / PI));
}

float JungleMath::DegToRad(float degree)
{
    return static_cast<float>(degree * (PI / 180.0f));
}
