#pragma once
#include "GizmoBaseComponent.h"
#include "UObject/ObjectTypes.h"

class UGizmoArrowComponent : public UGizmoBaseComponent
{
    DECLARE_CLASS(UGizmoArrowComponent, UGizmoBaseComponent)

public:
    UGizmoArrowComponent() = default;

    virtual void InitializeComponent() override;
    virtual void TickComponent(float DeltaTime) override;

private:
    EArrowDirection Dir;

public:
    EArrowDirection GetDir() const { return Dir; }
    void SetDir(const EArrowDirection InDir) { Dir = InDir; }
};
