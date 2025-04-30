#include "ExplosionShake.h"
#include "Math/Vector.h"
#include "UObject/UObjectArray.h"

UExplosionShake::UExplosionShake()
{
}

UExplosionShake::~UExplosionShake()
{
}

void UExplosionShake::MarkRemoveObject()
{
    GUObjectArray.MarkRemoveObject(this);
}

void UExplosionShake::UpdateShake(float DeltaTime, FVector& OutLoc, FRotator& OutRot, float& OutFOV)
{
    ElapsedTime += DeltaTime;
    float W = GetBlendWeight();

    float T = ElapsedTime * Frequency;

    float Pitch = FMath::Sin(T * 1.0f) * MaxPitch * W;
    float Yaw = FMath::Cos(T * 1.5f) * MaxYaw * W;
    float Roll = FMath::Sin(T * 0.8f + 1.2f) * MaxRoll * W;

    // 델타 적용
    OutRot.Pitch = Pitch - LastPitch;
    OutRot.Yaw = Yaw - LastYaw;
    OutRot.Roll = Roll - LastRoll;

    // 이전 값 저장
    LastPitch = Pitch;
    LastYaw = Yaw;
    LastRoll = Roll;

    OutLoc = FVector::ZeroVector;
    OutFOV = 0.f;
}