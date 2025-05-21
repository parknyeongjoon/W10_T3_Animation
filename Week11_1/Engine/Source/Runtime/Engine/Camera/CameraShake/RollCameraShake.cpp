#include "RollCameraShake.h"

URollCameraShake::URollCameraShake()
{
    Duration = 1.6f;
    BlendInTime = 0.f;
    BlendOutTime = 0.1f;
    TotalRotation = -360.f;
}

URollCameraShake::~URollCameraShake()
{
}

void URollCameraShake::UpdateShake(float DeltaTime, FVector& OutLoc, FRotator& OutRot, float& OutFOV)
{
    ElapsedTime += DeltaTime;
    float Alpha = FMath::Clamp(ElapsedTime / Duration, 0.f, 1.f);
    float W = GetBlendWeight();

    float Curve = FMath::InterpEaseOut(0.f, 1.f, Alpha, 3.0f);
    float CurrentPitch = Curve * TotalRotation * W;
    std::cout << "Curve : " << Curve << std::endl;
    if (ElapsedTime >= Duration + BlendOutTime)
    {
        float Remaining = TotalRotation - LastPitch;

        OutRot.Pitch = Remaining;
        LastPitch = TotalRotation;

        bFinished = true;
        return;
    }

    OutRot.Pitch = CurrentPitch - LastPitch;
    LastPitch = CurrentPitch;

    OutLoc = FVector::ZeroVector;
    OutFOV = 0.f;
}


