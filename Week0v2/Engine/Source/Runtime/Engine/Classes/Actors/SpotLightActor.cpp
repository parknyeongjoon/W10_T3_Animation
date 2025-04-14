#include "SpotLightActor.h"
#include "Components/SpotLightComponent.h"

ASpotLightActor::ASpotLightActor()
{
    LightComponent = AddComponent<USpotLightComponent>();
}

ASpotLightActor::ASpotLightActor(const ASpotLightActor& Other)
{
}
