#pragma once
#include "HAL/PlatformType.h"

struct FFrameTime
{
public:
    FFrameTime() : FrameNumber(0), SubFrame(0.f) {}
    FFrameTime(uint32 InFrameNumber) : FrameNumber(InFrameNumber), SubFrame(0.f) {}
    FFrameTime(uint32 InFrameNumber, float InSubFrame) : FrameNumber(InFrameNumber), SubFrame(InSubFrame) {}

    uint32 GetFrame() const { return FrameNumber; }
    float GetSubFrame() const { return SubFrame; }
    
private:
    uint32 FrameNumber;
    float SubFrame;
};