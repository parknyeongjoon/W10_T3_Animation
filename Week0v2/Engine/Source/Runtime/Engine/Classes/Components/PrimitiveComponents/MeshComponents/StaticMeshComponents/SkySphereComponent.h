#pragma once
#include "StaticMeshComponent.h"
#include "Engine/Texture.h"


class USkySphereComponent : public UStaticMeshComponent
{
    DECLARE_CLASS(USkySphereComponent, UStaticMeshComponent)

public:
    USkySphereComponent();
    USkySphereComponent(const USkySphereComponent& Other);
    virtual ~USkySphereComponent() override;

    virtual void InitializeComponent() override;
    virtual void TickComponent(float DeltaTime) override;
    float UOffset = 0;
    float VOffset = 0;

};
