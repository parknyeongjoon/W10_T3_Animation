#pragma once
#include "GizmoBaseComponent.h"

class UGizmoCircleComponent : public UGizmoBaseComponent
{
    DECLARE_CLASS(UGizmoCircleComponent, UGizmoBaseComponent)

public:
    UGizmoCircleComponent();
    virtual ~UGizmoCircleComponent() override;
    virtual bool IntersectsRay(const FVector& rayOrigin, const FVector& rayDir, float& dist);

    float GetInnerRadius() const { return inner; }
    void SetInnerRadius(float value) { inner = value; }

private:
    float inner = 1.0f;
};
