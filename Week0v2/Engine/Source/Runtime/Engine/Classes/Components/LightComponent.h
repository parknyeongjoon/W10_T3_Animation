#pragma once
#include "PrimitiveComponent.h"
#include "Define.h"
class UBillboardComponent;

class ULightComponentBase : public USceneComponent
{
    DECLARE_CLASS(ULightComponentBase, USceneComponent)

public:
    ULightComponentBase();
    ULightComponentBase(const ULightComponentBase& Other);
    virtual ~ULightComponentBase() override;

    virtual void TickComponent(float DeltaTime) override;
    virtual int CheckRayIntersection(FVector& rayOrigin, FVector& rayDirection, float& pfNearHitDistance);
    void InitializeLight();
    void SetColor(FVector4 newColor);
    FVector4 GetColor() const;

protected:
    FVector4 color = { 1, 1, 1, 1 }; // RGBA
    float Intensity = 1.0f;
    FBoundingBox AABB;
    UBillboardComponent* texture2D;
public:
    FBoundingBox GetBoundingBox() const {return AABB;}
    FVector4 GetColor() {return color;}
    UBillboardComponent* GetTexture2D() const {return texture2D;}
    float GetIntensity() const { return Intensity; }
    void SetIntensity(float _intensity) { Intensity = _intensity; }

public:
    // duplictae
    virtual UObject* Duplicate() const override;
    virtual void DuplicateSubObjects(const UObject* Source) override;
    virtual void PostDuplicate() override;
};
