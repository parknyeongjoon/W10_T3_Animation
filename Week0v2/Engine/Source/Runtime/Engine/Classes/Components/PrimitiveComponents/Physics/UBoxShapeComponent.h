#pragma once
#include "UShapeComponent.h"

class UBoxShapeComponent : public UShapeComponent
{
    DECLARE_CLASS(UBoxShapeComponent, UShapeComponent);

public:
    UBoxShapeComponent();
    UBoxShapeComponent(const UBoxShapeComponent& Other);
    ~UBoxShapeComponent() override;

    void InitializeComponent() override;
    void TickComponent(float DeltaTime) override;

    void SetBoxExtent(const FVector& InExtent) { BoxExtent = InExtent; }
    FVector GetBoxExtent() const { return BoxExtent; }

    void UpdateBroadAABB() override;

    virtual bool TestOverlaps(const UShapeComponent* OtherShape) const override;
    virtual bool NarrowPhaseCollisionCheck(const UShapeComponent* OtherShape) const override;

private:
    FVector BoxExtent;
};

