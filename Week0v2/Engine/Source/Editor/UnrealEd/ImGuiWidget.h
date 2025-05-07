#pragma once
#include <string>
#include "Math/Rotator.h"

struct FImGuiWidget
{
    static bool DrawVec3Control(const std::string& label, FVector& values, float resetValue = 0.0f, float columnWidth = 100.0f);


    static bool DrawRot3Control(const std::string& label, FRotator& values, float resetValue, float columnWidth);
};
