#pragma once
#include "PrimitiveComponent.h"
#include "Define.h"
class UBillboardComponent;

struct FLightComponentInfo : public FSceneComponentInfo
{
    FVector4 Color;
    FBoundingBox AABB;
    float Intensity;
    virtual void Copy(FActorComponentInfo& Other) override
    {
        FSceneComponentInfo::Copy(Other);
        FLightComponentInfo& LightInfo = static_cast<FLightComponentInfo&>(Other);
        LightInfo.Color = Color;
        LightInfo.AABB = AABB;
        LightInfo.Intensity = Intensity;
    }

    virtual void Serialize(FArchive& ar) const override
    {
        FSceneComponentInfo::Serialize(ar);
        ar << Color << AABB << Intensity;
    }

    virtual void Deserialize(FArchive& ar) override
    {
        FSceneComponentInfo::Deserialize(ar);
        ar >> Color >> AABB >> Intensity;
    }
};

class ULightComponentBase : public USceneComponent
{
    DECLARE_CLASS(ULightComponentBase, USceneComponent)

public:
    ULightComponentBase();
    ULightComponentBase(const ULightComponentBase& Other);
    virtual ~ULightComponentBase() override;

    virtual void TickComponent(float DeltaTime) override;
    virtual int CheckRayIntersection(FVector& rayOrigin, FVector& rayDirection, float& pfNearHitDistance);
    virtual void InitializeComponent() override;
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    void InitializeLight();
    void SetColor(FVector4 newColor);
    FVector4 GetColor() const;

protected:
    FVector4 color = { 1, 1, 1, 1 }; // RGBA
    float Intensity = 1.0f;
    FBoundingBox AABB;
public:
    FBoundingBox GetBoundingBox() const {return AABB;}
    FVector4 GetColor() {return color;}
    float GetIntensity() const { return Intensity; }
    void SetIntensity(float _intensity) { Intensity = _intensity; }

public:
    // duplictae
    virtual UObject* Duplicate() const override;
    virtual void DuplicateSubObjects(const UObject* Source) override;
    virtual void PostDuplicate() override;

public:
    virtual FActorComponentInfo GetActorComponentInfo() override;
    virtual void LoadAndConstruct(const FActorComponentInfo& Info) override;

};
