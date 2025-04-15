#include "SpotLightActor.h"
#include "Components/SpotLightComponent.h"
#include "Components/UBillboardComponent.h"

ASpotLightActor::ASpotLightActor()
{
    LightComponent = AddComponent<USpotLightComponent>();
    RootComponent->SetRelativeRotation(FVector(0, 89.0f, 0));
    BillboardComponent->SetTexture(L"Assets/Texture/SpotLight_64x.png");
}

ASpotLightActor::ASpotLightActor(const ASpotLightActor& Other)
{
}
