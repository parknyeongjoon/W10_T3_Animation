#pragma once
#include "Light.h"
class ADirectionalLightActor :
    public ALight
{
    DECLARE_CLASS(ADirectionalLightActor, ALight)
public:
    ADirectionalLightActor();
    ADirectionalLightActor(const ADirectionalLightActor& Other);
    virtual ~ADirectionalLightActor() override = default;
};

