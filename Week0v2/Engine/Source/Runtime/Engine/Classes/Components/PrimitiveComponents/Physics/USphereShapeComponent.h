#pragma once
#include "UShapeComponent.h"
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
    float Radius;
};

