#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

class USkinnedAsset : public UObject
{
    DECLARE_CLASS(USkinnedAsset, UObject)

public:
    USkinnedAsset() = default;
    virtual ~USkinnedAsset() override;

    // UObject* Duplicate() const override;
    // void DuplicateSubObjects(const UObject* Source) override;
    // void PostDuplicate() override;
    //

    virtual struct FReferenceSkeleton& GetRefSkeleton();
    virtual const struct FReferenceSkeleton& GetRefSkeleton() const;
};
