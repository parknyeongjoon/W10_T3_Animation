#include "PointLightActor.h"
#include <Components/PointLightComponent.h>

APointLightActor::APointLightActor()
{
    LightComponent = AddComponent<UPointLightComponent>();
}

APointLightActor::APointLightActor(const APointLightActor& Other)
{
}
