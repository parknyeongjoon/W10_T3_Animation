#pragma once
#include "LightComponent.h"

struct FDirectionalLightComponentInfo : public FLightComponentInfo
{
    DECLARE_ACTORCOMPONENT_INFO(FDirectionalLightComponentInfo);

    FVector Direction;

    FDirectionalLightComponentInfo()
        : FLightComponentInfo()
        , Direction(FVector(0.0f, 0.0f, -1.0f))
    {
        InfoType = TEXT("FDirectionalLightComponentInfo");
        ComponentType = TEXT("UDirectionalLightComponent");
    }

    virtual void Copy(FActorComponentInfo& Other) override
    {
        FLightComponentInfo::Copy(Other);
        FDirectionalLightComponentInfo& DirectionalLightInfo = static_cast<FDirectionalLightComponentInfo&>(Other);
        DirectionalLightInfo.Direction = Direction;
    }
    virtual void Serialize(FArchive& ar) const override
    {
        FLightComponentInfo::Serialize(ar);
        ar << Direction;
    }
    virtual void Deserialize(FArchive& ar) override
    {
        FLightComponentInfo::Deserialize(ar);
        ar >> Direction;
    }
};
class UDirectionalLightComponent :
    public ULightComponentBase
{
    DECLARE_CLASS(UDirectionalLightComponent, ULightComponentBase)
public:
    UDirectionalLightComponent();
    UDirectionalLightComponent(const UDirectionalLightComponent& Other);
    virtual ~UDirectionalLightComponent() override = default;
private:
    FVector Direction = FVector(0.0f, 0.0f, -1.0f);
public:
    FVector GetDirection() const { return Direction; }
    void SetDirection(FVector _newDir) { Direction = _newDir; }

public:
    virtual UObject* Duplicate() const override;
    virtual void DuplicateSubObjects(const UObject* Source) override;
    virtual void PostDuplicate() override;

public:
    virtual std::shared_ptr<FActorComponentInfo> GetActorComponentInfo() override;
    virtual void LoadAndConstruct(const FActorComponentInfo& Info) override;
};


