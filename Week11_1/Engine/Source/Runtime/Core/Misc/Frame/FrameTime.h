#pragma once
#include "HAL/PlatformType.h"

struct FFrameTime
{
public:
    FFrameTime() : FrameNumber(0), SubFrame(0.f) {}
    FFrameTime(int32 InFrameNumber) : FrameNumber(InFrameNumber), SubFrame(0.f) {}
    FFrameTime(int32 InFrameNumber, float InSubFrame) : FrameNumber(InFrameNumber), SubFrame(InSubFrame) {}

    int32 GetFrame() const { return FrameNumber; }
    float GetSubFrame() const { return SubFrame; }

    int32 FloorToFrame() const;
    int32 CeilToFrame() const;
    int32 RoundToFrame() const;
    
private:
    int32 FrameNumber;
    float SubFrame;
};

inline int32 FFrameTime::FloorToFrame() const
{
    return FrameNumber;
}


inline int32 FFrameTime::CeilToFrame() const
{
    return SubFrame == 0.f ? FrameNumber : FrameNumber+1;
}


inline int32 FFrameTime::RoundToFrame() const
{
    return SubFrame < .5f ? FrameNumber : FrameNumber+1;
}