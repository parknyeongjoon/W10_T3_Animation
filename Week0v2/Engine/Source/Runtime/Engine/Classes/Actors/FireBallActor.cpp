#include "FireBallActor.h"

#include "Components/PointLightComponent.h"
#include "Components/GameFramework/ProjectileMovementComponent.h"

AFireBallActor::AFireBallActor()
{
    FireBallComponent = AddComponent<UFireBallComponent>();
    LightComp = AddComponent<UPointLightComponent>();
    LightComp->SetIntensity(3.f);
    LightComp->SetRadius(20.f);
    ProjMovementComp = AddComponent<UProjectileMovementComponent>();
}

AFireBallActor::AFireBallActor(const AFireBallActor* Other)
{
}
