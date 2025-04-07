#include "DirectionalLightActor.h"
#include <Components/DirectionalLightComponent.h>

ADirectionalLightActor::ADirectionalLightActor()
{
    LightComponent = AddComponent<UDirectionalLightComponent>();
}

ADirectionalLightActor::ADirectionalLightActor(const ADirectionalLightActor& Other)
{
}
