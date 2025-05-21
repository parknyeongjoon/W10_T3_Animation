#include "UTextUUID.h"

UTextUUID::UTextUUID()
{
    SetRelativeScale(FVector(0.1f, 0.25f, 0.25f));
    SetRelativeLocation(FVector(0.0f, 0.0f, -0.5f));
}


UTextUUID::~UTextUUID()
{
}

int UTextUUID::CheckRayIntersection(FVector& rayOrigin, FVector& rayDirection, float& pfNearHitDistance)
{
    return 0;
}

void UTextUUID::SetUUID(uint32 UUID)
{
    SetText(std::to_wstring(UUID));
}


