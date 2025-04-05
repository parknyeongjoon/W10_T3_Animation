#pragma once
#include "LightComponent.h"
class UDirectionalLightComponent :
    public ULightComponentBase
{
    DECLARE_CLASS(UDirectionalLightComponent, ULightComponentBase)
public:
    UDirectionalLightComponent();
    UDirectionalLightComponent(const UDirectionalLightComponent& Other);
    virtual ~UDirectionalLightComponent() override = default;
private:
    FVector Direction;
public:
    FVector GetDirection() const { return Direction; }
    void SetDirection(FVector _newDir) { Direction = _newDir; }

public:
    virtual UObject* Duplicate() const override;
    virtual void DuplicateSubObjects(const UObject* Source) override;
    virtual void PostDuplicate() override;


};

