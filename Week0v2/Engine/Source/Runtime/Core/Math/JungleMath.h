#pragma once  
#include "Define.h"  
#include "Quat.h"  
#include "Rotator.h"  

class JungleMath
{
public:
    static FVector4 ConvertV3ToV4(FVector vec3);
    static FMatrix CreateModelMatrix(FVector translation, FVector rotation, FVector scale);
    static FMatrix CreateModelMatrix(FVector translation, FQuat rotation, FVector scale);
    static FMatrix CreateViewMatrix(FVector eye, FVector target, FVector up);
    static FMatrix CreateProjectionMatrix(float fov, float aspect, float nearPlane, float farPlane);
    static FMatrix CreateOrthoProjectionMatrix(float width, float height, float nearPlane, float farPlane);
    static FMatrix CreateOrthoProjectionMatrix(float left, float right, float bottom, float top, float nearZ, float farZ);

    static FVector FVectorRotate(FVector& origin, const FVector& rotation);
    static FVector FVectorRotate(FVector& origin, const FRotator& InRotation);
    static FMatrix CreateRotationMatrix(FVector rotation);
    static float   RadToDeg(float radian);
    static float DegToRad(float degree);
    static FQuat EulerToQuaternion(const FVector& eulerDegrees);
    static FVector QuaternionToEuler(const FQuat& quat);
    static FVector ClosestPointOnSegment(const FVector& A, const FVector& B, const FVector& Point);
    static void FindClosestPointsBetweenSegments(const FVector& P1, const FVector& Q1, const FVector& P2, const FVector& Q2, FVector& ClosestP1, FVector& ClosestP2);
};
