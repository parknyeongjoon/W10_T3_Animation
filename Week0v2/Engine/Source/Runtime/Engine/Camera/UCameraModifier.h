#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

class UCameraModifier : public UObject
{
    DECLARE_CLASS(UCameraModifier, UObject)
public:
    UCameraModifier();
    virtual ~UCameraModifier();
    uint8 Priority;
    virtual void ModifiyCamera();
    virtual void ModifiyPostProcess();
};
