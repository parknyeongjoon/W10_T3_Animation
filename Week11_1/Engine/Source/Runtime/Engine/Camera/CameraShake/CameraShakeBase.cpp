#include "CameraShakeBase.h"
#include "UObject/UObjectArray.h"

bool FActiveCameraShakeInfo::IsFinished() const
{
    return Instance == nullptr || Instance->bFinished;
}

UCameraShakeBase::UCameraShakeBase()
{
}

UCameraShakeBase::~UCameraShakeBase()
{
}

void UCameraShakeBase::MarkRemoveObject()
{
    GUObjectArray.MarkRemoveObject(this);
}

void UCameraShakeBase::UpdateShake(float DeltaTime, FVector& OutLocShake, FRotator& OutRotShake, float& OutFOVShake)
{
    OutLocShake = FVector::ZeroVector;
    OutRotShake = FRotator::ZeroRotator;
    OutFOVShake = 0.f;
}

void UCameraShakeBase::Tick(float DeltaTime)
{
    ElapsedTime += DeltaTime;
    if (ElapsedTime >= Duration + BlendOutTime)
    {
        bFinished = true;
    }
}

float UCameraShakeBase::GetBlendWeight() const
{
    if (ElapsedTime < BlendInTime)
    {
        return ElapsedTime / BlendInTime;
    }
    else if (ElapsedTime > Duration)
    {
        return 1.0f - ((ElapsedTime - Duration) / BlendOutTime);
    }
    return 1.0f;
}
