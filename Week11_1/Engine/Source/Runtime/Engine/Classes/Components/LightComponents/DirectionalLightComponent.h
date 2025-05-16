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
        ComponentClass = TEXT("UDirectionalLightComponent");
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
class UDirectionalLightComponent : public ULightComponent
{
    DECLARE_CLASS(UDirectionalLightComponent, ULightComponent)
public:
    UDirectionalLightComponent();
    UDirectionalLightComponent(const UDirectionalLightComponent& Other);
    virtual ~UDirectionalLightComponent() override;
private:
    FVector Direction = FVector(0.0f, 0.0f, -1.0f);
public:
    FVector GetDirection() { return Direction; }
    void SetDirection(FVector _newDir) { Direction = _newDir; }

    FMatrix GetViewMatrix() const override;
    FMatrix GetCascadeViewMatrix(UINT CascadeIndex) const;
    FMatrix GetProjectionMatrix() const override;
    FMatrix GetCascadeProjectionMatrix(UINT CascadeIndex) const;

public:
    virtual UObject* Duplicate(UObject* InOuter) override;
    virtual void DuplicateSubObjects(const UObject* Source, UObject* InOuter) override;
    virtual void PostDuplicate() override;

public:
    std::unique_ptr<FActorComponentInfo> GetComponentInfo() override;
    virtual void SaveComponentInfo(FActorComponentInfo& OutInfo) override;
    virtual void LoadAndConstruct(const FActorComponentInfo& Info) override;

private:
    TArray<FShadowResource*> ShadowResources;
};