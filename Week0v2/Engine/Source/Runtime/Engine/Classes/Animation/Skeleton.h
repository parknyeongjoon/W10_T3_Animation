#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

struct FRefSkeletal;
class USkeleton : public UObject
{
    DECLARE_CLASS(USkeleton, UObject)
public:
    USkeleton() = default;
    
    FRefSkeletal* GetRefSkeletal() const { return RefSkeletal; }
    void SetRefSkeletal(FRefSkeletal* InRefSkeletal) { RefSkeletal = InRefSkeletal; }
private:
    FRefSkeletal* RefSkeletal;
};
