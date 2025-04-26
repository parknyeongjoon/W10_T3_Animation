#pragma once
#include "Components/PrimitiveComponents/PrimitiveComponent.h"
#include "Math/Vector.h"
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
    virtual bool NarrowPhaseCollisionCheck(const UShapeComponent* OtherShape) const;

protected:
    virtual void UpdateBroadAABB() {}

protected:
    FBoundingBox BroadAABB;
    FVector PrevLocation;
    FRotator PrevRotation;
    FVector PrevScale;

private:
    FLinearColor ShapeColor;
    bool bDrawOnlyIfSelected;

};

 