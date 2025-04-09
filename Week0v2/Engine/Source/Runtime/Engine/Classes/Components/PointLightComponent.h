#pragma once
#include "LightComponent.h"

struct FPointLightComponentInfo : public FLightComponentInfo
{
    DECLARE_ACTORCOMPONENT_INFO(FPointLightComponentInfo);

    float Radius;
    float AttenuationFalloff;

    FPointLightComponentInfo()
        : FLightComponentInfo()
        , Radius(1.0f)
        , AttenuationFalloff(0.01f)
    {
        InfoType = TEXT("FPointLightComponentInfo");
        ComponentType = TEXT("UPointLightComponent");
    }

    virtual void Copy(FActorComponentInfo& Other) override
    {
        FLightComponentInfo::Copy(Other);
        FPointLightComponentInfo& PointLightInfo = static_cast<FPointLightComponentInfo&>(Other);
        PointLightInfo.Radius = Radius;
        PointLightInfo.AttenuationFalloff = AttenuationFalloff;
    }

    virtual void Serialize(FArchive& ar) const override
    {
        FLightComponentInfo::Serialize(ar);
        ar << Radius << AttenuationFalloff;
    }

    virtual void Deserialize(FArchive& ar) override
    {
        FLightComponentInfo::Deserialize(ar);
        ar >> Radius >> AttenuationFalloff;
    }
};

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

public:
    virtual std::shared_ptr<FActorComponentInfo> GetActorComponentInfo() override;
    virtual void LoadAndConstruct(const FActorComponentInfo& Info) override;

};

