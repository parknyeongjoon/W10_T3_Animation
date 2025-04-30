#include "GunRecoilShake.h"
#include "Curves/CurveFloat.h"
#include "UObject/UObjectArray.h"

UGunRecoilShake::UGunRecoilShake()
{
}

UGunRecoilShake::~UGunRecoilShake()
{

}

void UGunRecoilShake::MarkRemoveObject()
{
    GUObjectArray.MarkRemoveObject(this);
    if (PitchCurve) GUObjectArray.MarkRemoveObject(PitchCurve);
    if (YawCurve) GUObjectArray.MarkRemoveObject(YawCurve);
    if (RollCurve) GUObjectArray.MarkRemoveObject(RollCurve);
}

void UGunRecoilShake::UpdateShake(float DeltaTime, FVector& OutLoc, FRotator& OutRot, float& OutFOV)
{
    ElapsedTime += DeltaTime;
    float T = ElapsedTime;
    float W = GetBlendWeight();

    float Pitch = PitchCurve ? PitchCurve->GetFloatValue(T) * Scale * W : 0.f;
    float Yaw = YawCurve ? YawCurve->GetFloatValue(T) * Scale * W : 0.f;
    float Roll = RollCurve ? RollCurve->GetFloatValue(T) * Scale * W : 0.f;

    OutRot.Pitch = Pitch - LastPitch;
    OutRot.Yaw = Yaw - LastYaw;
    OutRot.Roll = Roll - LastRoll;

    LastPitch = Pitch;
    LastYaw = Yaw;
    LastRoll = Roll;

    OutLoc = FVector::ZeroVector;
    OutFOV = 0.f;
}