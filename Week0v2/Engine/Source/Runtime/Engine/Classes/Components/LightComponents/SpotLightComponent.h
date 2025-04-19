#pragma once
#include "DirectionalLightComponent.h"

struct FSpotlightComponentInfo : public FDirectionalLightComponentInfo
{
    DECLARE_ACTORCOMPONENT_INFO(FSpotlightComponentInfo);

    float InnerConeAngle;
    float OuterConeAngle;

    FSpotlightComponentInfo()
        :FDirectionalLightComponentInfo()
        , InnerConeAngle(0.0f)
        , OuterConeAngle(0.768f)
    {
        InfoType = TEXT("FSpotLightComponentInfo");
        ComponentType = TEXT("USpotLightComponent");
    }

    virtual void Copy(FActorComponentInfo& Other) override
    {
        FDirectionalLightComponentInfo::Copy(Other);
        FSpotlightComponentInfo& SpotLightInfo = static_cast<FSpotlightComponentInfo&>(Other);
        SpotLightInfo.InnerConeAngle = InnerConeAngle;
        SpotLightInfo.OuterConeAngle = OuterConeAngle;
    }

    virtual void Serialize(FArchive& ar) const override
    {
        FDirectionalLightComponentInfo::Serialize(ar);
        ar << InnerConeAngle << OuterConeAngle;
    }

    virtual void Deserialize(FArchive& ar) override
    {
        FDirectionalLightComponentInfo::Deserialize(ar);
        ar >> InnerConeAngle >> OuterConeAngle;
    }
};

class USpotLightComponent : public UDirectionalLightComponent
{
    DECLARE_CLASS(USpotLightComponent, UDirectionalLightComponent)
public:
    USpotLightComponent();
    USpotLightComponent(const USpotLightComponent& Other);
    virtual ~USpotLightComponent() override;
protected:
    //angle은 내부적으로 radian
    float InnerConeAngle = 0.0f;
    float OuterConeAngle = 0.768f;

public:
    float GetInnerConeAngle() const { return InnerConeAngle; }
    float GetOuterConeAngle() const { return OuterConeAngle; }
    //외부에서 set 해줄때는 degree로 들어옴
    void SetInnerConeAngle(float Angle);
    void SetOuterConeAngle(float Angle);
    virtual UObject* Duplicate() const override;
    virtual void DuplicateSubObjects(const UObject* Source) override;
    virtual void PostDuplicate() override;
    FMatrix GetViewMatrix();
    FMatrix GetProjectionMatrix() const;

public:
    virtual std::shared_ptr<FActorComponentInfo> GetActorComponentInfo() override;
    virtual void LoadAndConstruct(const FActorComponentInfo& Info) override;
};