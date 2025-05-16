#pragma once
#include "UShapeComponent.h"
#include "UCapsuleShapeComponent.h"
#include "UBoxShapeComponent.h"

class UBoxShapeComponent;
class UCapsuleShapeComponent;

struct FSphereShapeInfo : public FShapeInfo
{
    FSphereShapeInfo()
        : FShapeInfo(EShapeType::Sphere
            , FVector::ZeroVector
            , FMatrix::Identity)
        , Radius(0.0f) 
    {
    }

    FSphereShapeInfo(const FVector& InCenter, const FMatrix& InWorldMatrix, float InRadius)
        : FShapeInfo(EShapeType::Sphere, InCenter, InWorldMatrix), Radius(InRadius) 
    {
    }

    float Radius;
};

class USphereShapeComponent : public UShapeComponent
{
    DECLARE_CLASS(USphereShapeComponent, UShapeComponent);

public:
    USphereShapeComponent();
    USphereShapeComponent(const USphereShapeComponent& Other);
    virtual ~USphereShapeComponent() override;

    void InitializeComponent() override;
    void TickComponent(float DeltaTime) override;

    virtual UObject* Duplicate(UObject* InOuter) override;
    virtual void DuplicateSubObjects(const UObject* Source, UObject* InOuter) override;
    virtual void PostDuplicate() override;

    void SetRadius(float InRadius) { Radius = InRadius; }
    float GetRadius() const { return Radius; }

    virtual const FShapeInfo* GetShapeInfo() const override;
    virtual void UpdateBroadAABB() override;

    virtual bool TestOverlaps(const UShapeComponent* OtherShape) const override;
    virtual bool NarrowPhaseCollisionCheck(const UShapeComponent* OtherShape) const override;

private:
    bool CollisionCheckWithBox(const UBoxShapeComponent* OtherBox) const;
    bool CollisionCheckWithSphere(const USphereShapeComponent* OtherSphere) const;
    bool CollisionCheckWithCapsule(const UCapsuleShapeComponent* OtherCapsule) const;

protected:
    mutable FSphereShapeInfo ShapeInfo;

private:
    float PrevRadius;
    float Radius;
};

