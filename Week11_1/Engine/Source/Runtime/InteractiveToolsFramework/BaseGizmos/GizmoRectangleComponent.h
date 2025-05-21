#pragma once
#include "GizmoBaseComponent.h"


class UGizmoRectangleComponent : public UGizmoBaseComponent
{
    DECLARE_CLASS(UGizmoRectangleComponent, UGizmoBaseComponent)

public:
    UGizmoRectangleComponent();
    virtual ~UGizmoRectangleComponent() override;

    virtual void InitializeComponent() override;
    virtual void TickComponent(float DeltaTime) override;
};
