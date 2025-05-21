#pragma once
#include "Light.h"

class ASpotLightActor : public ALight
{
    DECLARE_CLASS(ASpotLightActor, ALight)
public:
    ASpotLightActor();
    virtual ~ASpotLightActor() override = default;
};