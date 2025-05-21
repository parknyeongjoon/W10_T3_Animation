#include "DashShake.h"

UDashShake::UDashShake()
{
    Duration = 0.4f;
    BlendInTime = 0.f;
    BlendOutTime = 0.1f;
    ForwardOffset = 25.f;
    RollAmount = 3.f;
    Frequency = 15.f;
}

UDashShake::~UDashShake()
{
}

void UDashShake::UpdateShake(float DeltaTime, FVector& OutLoc, FRotator& OutRot, float& OutFOV)
{
    ElapsedTime += DeltaTime;
    float W = GetBlendWeight();
    float T = ElapsedTime;

    float CurveValue = FMath::InterpEaseOut(1.0f, 0.0f, T / Duration, 3.0f);  // 빠르게 튀고 서서히 복귀

    float ForwardDelta = ForwardOffset * CurveValue * W;
    float RollDelta = FMath::Sin(T * Frequency) * RollAmount * W;

    OutLoc = FVector(ForwardDelta - LastOffset, 0.f, 0.f);
    OutRot.Roll = RollDelta - LastRoll;

    LastOffset = ForwardDelta;
    LastRoll = RollDelta;

    OutFOV = 0.f;
}
