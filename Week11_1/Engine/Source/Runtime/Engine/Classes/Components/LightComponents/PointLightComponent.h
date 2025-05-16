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
        ComponentClass = TEXT("UPointLightComponent");
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
    void SetRadius(const float InRadius) { Radius = InRadius; }
    float GetAttenuation() const { return 1.0f / AttenuationFalloff * (Radius * Radius); }
    float GetAttenuationFalloff() const { return AttenuationFalloff; }
    void SetAttenuationFallOff(const float InAttenuationFalloff) { AttenuationFalloff = InAttenuationFalloff; }
    virtual UObject* Duplicate(UObject* InOuter) override;
    virtual void DuplicateSubObjects(const UObject* Source, UObject* InOuter) override;
    virtual void PostDuplicate() override;

    FMatrix GetViewMatrixForFace(int faceIndex) const;
    FMatrix GetProjectionMatrix() const override;

public:
    
    std::unique_ptr<FActorComponentInfo> GetComponentInfo() override;
    virtual void SaveComponentInfo(FActorComponentInfo& OutInfo) override;
    virtual void LoadAndConstruct(const FActorComponentInfo& Info) override;

};

