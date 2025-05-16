#pragma once
#include "LightComponent.h"

struct FSpotlightComponentInfo : public FLightComponentBaseInfo
{
    DECLARE_ACTORCOMPONENT_INFO(FSpotlightComponentInfo);

    float InnerConeAngle;
    float OuterConeAngle;
    FVector Direction;

    FSpotlightComponentInfo()
        :FLightComponentBaseInfo()
        , InnerConeAngle(0.0f)
        , OuterConeAngle(0.768f)
        , Direction(FVector(0.0f, 0.0f, -1.0f))
    {
        InfoType = TEXT("FSpotLightComponentInfo");
        ComponentClass = TEXT("USpotLightComponent");
    }

    virtual void Serialize(FArchive& ar) const override
    {
        FLightComponentBaseInfo::Serialize(ar);
        ar << InnerConeAngle << OuterConeAngle << Direction;
    }

    virtual void Deserialize(FArchive& ar) override
    {
        FLightComponentBaseInfo::Deserialize(ar);
        ar >> InnerConeAngle >> OuterConeAngle >> Direction;
    }
};

class USpotLightComponent : public ULightComponentBase
{
    DECLARE_CLASS(USpotLightComponent, ULightComponentBase)
public:
    USpotLightComponent() {};
    virtual ~USpotLightComponent() override;
protected:
    //angle은 내부적으로 radian
    float InnerConeAngle = 0.0f;
    float OuterConeAngle = 0.768f;

private:
    FVector Direction = FVector(0.0f, 0.0f, -1.0f);
public:
    FVector GetDirection() { return Direction; }
    void SetDirection(FVector _newDir) { Direction = _newDir; }

public:
    float GetInnerConeAngle() const { return InnerConeAngle; }
    float GetOuterConeAngle() const { return OuterConeAngle; }
    //외부에서 set 해줄때는 degree로 들어옴
    void SetInnerConeAngle(float Angle);
    void SetOuterConeAngle(float Angle);
    virtual UObject* Duplicate(UObject* InOuter) override;
    virtual void DuplicateSubObjects(const UObject* Source, UObject* InOuter) override;
    virtual void PostDuplicate() override;
    FMatrix GetViewMatrix() const override;
    FMatrix GetProjectionMatrix() const override;

public:
    
    std::unique_ptr<FActorComponentInfo> GetComponentInfo() override;
    virtual void SaveComponentInfo(FActorComponentInfo& OutInfo) override;
    virtual void LoadAndConstruct(const FActorComponentInfo& Info) override;

public:
    FVector4 GetLightAtlasUV() const;
};