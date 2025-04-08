#pragma once
#include "Components/FireBallComponent.h"
#include "GameFramework/Actor.h"

class AFireBallActor: public AActor
{
    DECLARE_CLASS(AFireBallActor, AActor)
public:
    AFireBallActor();
    AFireBallActor(const AFireBallActor* Other);
    virtual ~AFireBallActor() override = default;
private:
    UFireBallComponent* FireBallComponent;
};
