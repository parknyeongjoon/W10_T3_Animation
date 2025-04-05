#pragma once
#include "LightComponent.h"
class UPointLightComponent : public ULightComponentBase
{
    DECLARE_CLASS(UPointLightComponent, ULightComponentBase)
public:
    UPointLightComponent();
    UPointLightComponent(const UPointLightComponent& Other);
    virtual ~UPointLightComponent() override = default;
protected:
    float Radius;

public:
    float GetRadius() const { return Radius; }
    virtual UObject* Duplicate() const override;
    virtual void DuplicateSubObjects(const UObject* Source) override;
    virtual void PostDuplicate() override;
};

