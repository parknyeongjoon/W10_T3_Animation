#pragma once
#include "SphereComp.h"

class UFireBallComponent: public USphereComp
{
    DECLARE_CLASS(UFireBallComponent, USphereComp)
public:
    UFireBallComponent();
    virtual ~UFireBallComponent() override;
    void InitializeComponent() override;
    void TickComponent(float DeltaTime) override;
};
