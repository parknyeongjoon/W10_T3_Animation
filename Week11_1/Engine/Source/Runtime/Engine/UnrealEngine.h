#pragma once
#include "HAL/PlatformType.h"

class IEngineLoop
{
public:

    virtual int32 Init(HINSTANCE hInstance) = 0;

    virtual void Tick() = 0;

    /** Removes references to any objects pending cleanup by deleting them. */
    virtual void ClearPendingCleanupObjects() = 0;
};
