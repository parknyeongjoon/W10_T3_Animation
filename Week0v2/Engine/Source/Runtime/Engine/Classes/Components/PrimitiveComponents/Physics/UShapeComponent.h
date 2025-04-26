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

    // Getter, Setter
    FLinearColor GetShapeColor() const { return ShapeColor; }
    void SetShapeColor(const FLinearColor& InColor) { ShapeColor = InColor; }
    FBoundingBox GetBroadAABB() const { return BroadAABB; }
    bool IsDrawOnlyIfSelected() const { return bDrawOnlyIfSelected; }
    void SetbDrawOnlyIfSelected(bool bInDraw) { bDrawOnlyIfSelected = bInDraw; }

    FVector GetPrevLocation() const { return PrevLocation; }
    FRotator GetPrevRotation() const { return PrevRotation; }
    FVector GetPrevScale() const { return PrevScale; }

    virtual bool TestOverlaps(const UShapeComponent* OtherShape) const;
    virtual bool BroadPhaseCollisionCheck(const UShapeComponent* OtherShape) const;
    virtual bool NarrowPhaseCollisionCheck(const UShapeComponent* OtherShape) const { return false; }

protected:
    virtual void UpdateBroadAABB() {}

    FVector PrevLocation;
    FRotator PrevRotation;
    FVector PrevScale;

protected:
    FBoundingBox BroadAABB;

private:
    FLinearColor ShapeColor;
    bool bDrawOnlyIfSelected;
};

 