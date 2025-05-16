#pragma once
#include "Components/PrimitiveComponents/MeshComponents/StaticMeshComponents/StaticMeshComponent.h"

class UGizmoBaseComponent : public UStaticMeshComponent
{
    DECLARE_CLASS(UGizmoBaseComponent, UStaticMeshComponent)

public:
    enum EGizmoType : uint8
    {
        ArrowX,
        ArrowY,
        ArrowZ,
        CircleX,
        CircleY,
        CircleZ,
        ScaleX,
        ScaleY,
        ScaleZ
    };
public:
    UGizmoBaseComponent() = default;

    virtual int CheckRayIntersection(FVector& RayOrigin, FVector& RayDirection, float& pNearHitDistance) override;
    virtual void TickComponent(float DeltaTime) override;

private:
    EGizmoType GizmoType;

public:
    EGizmoType GetGizmoType() const { return GizmoType; }
    void SetGizmoType(const EGizmoType InGizmoType) { GizmoType = InGizmoType; }

    float GizmoScale = 0.2f;
};
