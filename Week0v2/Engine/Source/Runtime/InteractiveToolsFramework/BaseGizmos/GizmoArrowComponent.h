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
    ARROW_DIR Dir;

public:
    ARROW_DIR GetDir() const { return Dir; }
    void SetDir(ARROW_DIR _Dir) { Dir = _Dir; }
};
