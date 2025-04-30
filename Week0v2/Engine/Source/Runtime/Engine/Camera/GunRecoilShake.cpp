#include "GunRecoilShake.h"

void UGunRecoilShake::UpdateShake(float DeltaTime, FVector& OutLoc, FRotator& OutRot, float& OutFOV)
{
    Elapsed += DeltaTime;
    float Alpha = Elapsed / Duration;

    float Curve = FMath::InterpEaseOut(1.0f, 0.0f, Alpha, 2.0f); // 감쇠 커브

    OutRot.Pitch = TotalRecoil * Curve;
    OutRot.Yaw = 0.f;
    OutRot.Roll = 0.f;

    OutLoc = FVector::ZeroVector;
    OutFOV = 0.f;
}