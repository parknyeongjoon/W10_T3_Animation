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
    float Radius = 1.0f;
    float AttenuationFalloff = 0.01f;

public:
    float GetRadius() const { return Radius; }
    void SetRadius(float _newRadius) { Radius = _newRadius; }
    float GetAttenuation() const { return 1.0f / AttenuationFalloff * (Radius * Radius); }
    float GetAttenuationFalloff() const { return AttenuationFalloff; }
    void SetAttenuationFallOff(float _attenuationFalloff) { AttenuationFalloff = _attenuationFalloff; }
    virtual UObject* Duplicate() const override;
    virtual void DuplicateSubObjects(const UObject* Source) override;
    virtual void PostDuplicate() override;
};

