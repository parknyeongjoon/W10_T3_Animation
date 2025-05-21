#pragma once
#include "AnimSequenceBase.h"

struct FRefSkeletal;
class FTransform;

class UAnimSequence : public UAnimSequenceBase
{
    DECLARE_CLASS(UAnimSequence, UAnimSequenceBase)
public:
    UAnimSequence() = default;

    virtual UObject* Duplicate(UObject* InOuter) override;
    virtual void DuplicateSubObjects(const UObject* Source, UObject* InOuter) override;
    virtual void PostDuplicate() override;
    
private:
    EAnimInterpolationType InterpolationType;
};

