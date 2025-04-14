#pragma once
#include "Light.h"

class ASpotLightActor : public ALight
{
    DECLARE_CLASS(ASpotLightActor, ALight)
public:
    ASpotLightActor();
    ASpotLightActor(const ASpotLightActor& Other);
    virtual ~ASpotLightActor() override = default;
};