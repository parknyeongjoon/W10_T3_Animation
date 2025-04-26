#pragma once
#include "UShapeComponent.h"
class USphereShapeComponent : public UShapeComponent
{
    DECLARE_CLASS(USphereShapeComponent, UShapeComponent);

public:
    USphereShapeComponent();
    USphereShapeComponent(const USphereShapeComponent& Other);
    ~USphereShapeComponent() override;

    void InitializeComponent() override;
    void TickComponent(float DeltaTime) override;

    void SetRadius(float InRadius) { Radius = InRadius; }
    float GetRadius() const { return Radius; }

private:
    float Radius;
};

