#pragma once
#include "SphereComp.h"

class UFireBallComponent: public USphereComp
{
    DECLARE_CLASS(UFireBallComponent, USphereComp)
public:
    UFireBallComponent() = default;
    UFireBallComponent(const UFireBallComponent&);
    virtual ~UFireBallComponent() override;
    void InitializeComponent() override;
    void TickComponent(float DeltaTime) override;
    virtual UObject* Duplicate(UObject* InOuter) override;
    virtual void DuplicateSubObjects(const UObject* Source, UObject* InOuter) override;
    virtual void PostDuplicate() override;
};
