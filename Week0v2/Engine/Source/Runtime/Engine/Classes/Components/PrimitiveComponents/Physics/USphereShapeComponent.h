#pragma once
#include "UShapeComponent.h"
#include "UCapsuleShapeComponent.h"
#include "UBoxShapeComponent.h"

class UBoxShapeComponent;
class UCapsuleShapeComponent;

class USphereShapeComponent : public UShapeComponent
{
    DECLARE_CLASS(USphereShapeComponent, UShapeComponent);

public:
    USphereShapeComponent();
    USphereShapeComponent(const USphereShapeComponent& Other);
    virtual ~USphereShapeComponent() override;

    void InitializeComponent() override;
    void TickComponent(float DeltaTime) override;

    void SetRadius(float InRadius) { Radius = InRadius; }
    float GetRadius() const { return Radius; }

    virtual void UpdateBroadAABB() override;

    virtual bool TestOverlaps(const UShapeComponent* OtherShape) const override;
    virtual bool NarrowPhaseCollisionCheck(const UShapeComponent* OtherShape) const override;

private:
    bool CollisionCheckWithBox(const UBoxShapeComponent* OtherBox) const;
    bool CollisionCheckWithSphere(const USphereShapeComponent* OtherSphere) const;
    bool CollisionCheckWithCapsule(const UCapsuleShapeComponent* OtherCapsule) const;

private:
    float Radius;
};

