#pragma once
#include "Components/FireBallComponent.h"
#include "GameFramework/Actor.h"

class UBillboardComponent;
class UProjectileMovementComponent;
class UPointLightComponent;
class AFireBallActor: public AActor
{
    DECLARE_CLASS(AFireBallActor, AActor)
public:
    AFireBallActor();
    AFireBallActor(const AFireBallActor* Other);
    virtual ~AFireBallActor() override = default;
private:
    UFireBallComponent* FireBallComponent;
    UPointLightComponent* LightComp;
    UProjectileMovementComponent* ProjMovementComp;
    UBillboardComponent* Texture2D;
};
