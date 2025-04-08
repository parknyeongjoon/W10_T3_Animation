#pragma once
#include "Classes/GameFramework/Actor.h"
#include <Components/LightComponent.h>
class ALight : public AActor
{
    DECLARE_CLASS(ALight, AActor)
public:
    ALight();
    ALight(const ALight& Other);
    virtual ~ALight() override;

protected:
    ULightComponentBase* LightComponent;
    UBillboardComponent* Texture2D;

public:
    // duplicate
};

