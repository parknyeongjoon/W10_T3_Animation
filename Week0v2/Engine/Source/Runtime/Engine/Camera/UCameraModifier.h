#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

class UCameraModifier : public UObject
{
    DECLARE_CLASS(UCameraModifier, UObject)
public:
    uint8 Priority;
    virtual void ModifiyCamera();
    virtual void ModifiyPostProcess();
};
