#pragma once
#include "UTextComponent.h"

class UTextUUID : public UTextComponent
{
    DECLARE_CLASS(UTextUUID, UTextComponent)

public:
    UTextUUID();
    virtual ~UTextUUID() override;

    virtual int CheckRayIntersection(
        FVector& rayOrigin,
        FVector& rayDirection, float& pfNearHitDistance
    ) override;
    void SetUUID(uint32 UUID);
};
