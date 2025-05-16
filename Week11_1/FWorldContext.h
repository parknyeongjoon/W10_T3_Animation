#pragma once
#include "Core/HAL/PlatformType.h"
#include "World.h"

enum class EWorldType
{
    Editor,
    PIE,
};

class FWorldContext
{
public:
    UWorld* World() { return world; }


private:
    EWorldType type;
    UWorld* world;
};


