#pragma once
#include "HAL/PlatformType.h"

class UEngineStatics
{
public:
    UEngineStatics();
    ~UEngineStatics();
    static uint32 GenUUID();
    static uint32 NextUUID;
};

