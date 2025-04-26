#pragma once
#include "Components/PrimitiveComponents/PrimitiveComponent.h"
#include "Math/Color.h"

class UShapeComponent : public UPrimitiveComponent
{
    DECLARE_CLASS(UShapeComponent, UPrimitiveComponent);

public:
    UShapeComponent();
    UShapeComponent(const UShapeComponent& Other);
    ~UShapeComponent() override;

    void InitializeComponent() override;
    void TickComponent(float DeltaTime) override;

    virtual UObject* Duplicate() const override;
    virtual void DuplicateSubObjects(const UObject* Source) override;
    virtual void PostDuplicate() override;

    void SetShapeColor(const FLinearColor& InColor) { ShapeColor = InColor; }
    FLinearColor GetShapeColor() const { return ShapeColor; }

    virtual bool TestOverlaps(const UShapeComponent* OtherShape) const;
    virtual bool BroadPhaseCollisionCheck(const UShapeComponent* OtherShape) const;
    virtual bool NarrowPhaseCollisionCheck(const UShapeComponent* OtherShape) const;

private:
    FLinearColor ShapeColor;
    bool bDrawOnlyIfSelected;
};

