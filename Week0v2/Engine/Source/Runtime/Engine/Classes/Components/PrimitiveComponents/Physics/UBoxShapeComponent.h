#pragma once
#include "UShapeComponent.h"

class UBoxShapeComponent : public UShapeComponent
{
    DECLARE_CLASS(UBoxShapeComponent, UShapeComponent);

public:
    UBoxShapeComponent();
    UBoxShapeComponent(const UBoxShapeComponent& Other);
    ~UBoxShapeComponent() override;

    void InitializeComponent() override;
    void TickComponent(float DeltaTime) override;

    void SetBoxExtent(const FVector& InExtent) { BoxExtent = InExtent; }
    FVector GetBoxExtent() const { return BoxExtent; }

private:
    FVector BoxExtent;
};

