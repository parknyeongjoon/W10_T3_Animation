#pragma once
#include "UShapeComponent.h"

class UCapsuleShapeComponent : public UShapeComponent
{
    DECLARE_CLASS(UCapsuleShapeComponent, UShapeComponent);

public:
    UCapsuleShapeComponent();
    UCapsuleShapeComponent(const UCapsuleShapeComponent& Other);
    ~UCapsuleShapeComponent() override;

    void InitializeComponent() override;
    void TickComponent(float DeltaTime) override;

    void SetRadius(float InRadius) { CapsuleRaidus = InRadius; }
    float GetRadius() const { return CapsuleRaidus; }
    void SetHalfHeight(float InHeight) { CapsuleHalfHeight = InHeight; }
    float GetHalfHeight() const { return CapsuleHalfHeight; }

    virtual bool TestOverlaps(const UShapeComponent* OtherShape) const override;
    virtual bool NarrowPhaseCollisionCheck(const UShapeComponent* OtherShape) const override;

public:


private:
    float CapsuleHalfHeight;
    float CapsuleRaidus;
};

