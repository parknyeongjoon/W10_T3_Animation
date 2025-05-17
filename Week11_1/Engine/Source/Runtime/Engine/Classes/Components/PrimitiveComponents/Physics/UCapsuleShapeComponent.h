#pragma once
#include "UShapeComponent.h"
#include "UBoxShapeComponent.h"
#include "USphereShapeComponent.h"

struct FCapsuleShapeInfo : public FShapeInfo
{
    FCapsuleShapeInfo()
        : FShapeInfo(EShapeType::Capsule, FVector::ZeroVector, FMatrix::Identity)
        , Up(FVector::UpVector)
        , RotationMatrix(FMatrix::Identity)
        , Radius(0.0f)
        , HalfHeight(0.0f)
    {
    }

    FCapsuleShapeInfo(const FVector& InCenter, const FMatrix& InWorldMatrix, float InRadius, float InHalfHeight)
        : FShapeInfo(EShapeType::Capsule, InCenter, InWorldMatrix), Up(FVector::UpVector), Radius(InRadius), HalfHeight(InHalfHeight)
    {
    }

    FVector Up;
    FMatrix RotationMatrix;
    float Radius;
    float HalfHeight;
};

class UCapsuleShapeComponent : public UShapeComponent
{
    DECLARE_CLASS(UCapsuleShapeComponent, UShapeComponent);

public:
    UCapsuleShapeComponent();
    ~UCapsuleShapeComponent() override;

    UObject* Duplicate(UObject* InOuter) override;

    void InitializeComponent() override;
    void TickComponent(float DeltaTime) override;

    void SetRadius(float InRadius) { CapsuleRadius = InRadius; }
    float GetRadius() const { return CapsuleRadius; }
    void SetHalfHeight(float InHeight) { CapsuleHalfHeight = InHeight; }
    float GetHalfHeight() const { return CapsuleHalfHeight; }

    virtual const FShapeInfo* GetShapeInfo() const override;
    virtual void UpdateBroadAABB() override;

    virtual bool TestOverlaps(const UShapeComponent* OtherShape) const override;
    virtual bool NarrowPhaseCollisionCheck(const UShapeComponent* OtherShape) const override;

private:
    bool CollisionCheckWithBox(const UBoxShapeComponent* OtherBox) const;
    bool CollisionCheckWithSphere(const USphereShapeComponent* OtherSphere) const;
    bool CollisionCheckWithCapsule(const UCapsuleShapeComponent* OtherCapsule) const;

protected:
    mutable FCapsuleShapeInfo ShapeInfo;

private:
    float PrevHalfHeight;
    float PrevRadius;

    float CapsuleHalfHeight;
    float CapsuleRadius;
};

