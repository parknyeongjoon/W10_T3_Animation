#include "FireBallActor.h"

#include "Components/PointLightComponent.h"
#include "Components/GameFramework/ProjectileMovementComponent.h"
#include "Components/UBillboardComponent.h"

AFireBallActor::AFireBallActor()
{
    FireBallComponent = AddComponent<UFireBallComponent>();
    LightComp = AddComponent<UPointLightComponent>();
    LightComp->SetIntensity(3.f);
    LightComp->SetRadius(20.f);
    ProjMovementComp = AddComponent<UProjectileMovementComponent>();
    Texture2D = AddComponent<UBillboardComponent>();
    Texture2D->SetTexture(L"Assets/Texture/spotLight.png");
}

AFireBallActor::AFireBallActor(const AFireBallActor* Other)
{
}
