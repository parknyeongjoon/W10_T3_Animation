#include "SpotLightActor.h"
#include "Components/SpotLightComponent.h"

ASpotLightActor::ASpotLightActor()
{
    LightComponent = AddComponent<USpotLightComponent>();
    RootComponent->SetRelativeRotation(FVector(0, 89.0f, 0));
}

ASpotLightActor::ASpotLightActor(const ASpotLightActor& Other)
{
}
