#pragma once
#include "Light.h"
class APointLightActor :
    public ALight
{
    DECLARE_CLASS(APointLightActor, ALight)
public:
    APointLightActor();
    APointLightActor(const APointLightActor& Other);
    virtual ~APointLightActor() override = default;
};

