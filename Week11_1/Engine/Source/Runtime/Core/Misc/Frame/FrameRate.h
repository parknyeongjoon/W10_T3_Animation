#pragma once
#include "FrameTime.h"
#include "HAL/PlatformType.h"
#include "Runtime/Serialization/Archive.h"

struct FFrameRate
{
    /**
     * Default construction to a frame rate of 60000 frames per second (0.0166 ms)
     */
    FFrameRate()
        : Numerator(60000), Denominator(1)
    {}

    FFrameRate(uint32 InNumerator, uint32 InDenominator)
        : Numerator(InNumerator), Denominator(InDenominator)
    {}
    
    int32 Numerator; 
    int32 Denominator;
    
    // Verify that this frame rate is valid to use
    bool IsValid() const
    {
        return Denominator > 0;
    }

    /**
     * Get the decimal representation of this framerate's interval
     * @return The time in seconds for a single frame under this frame rate
     */
    double AsInterval() const;

    /**
     * Get the decimal representation of this framerate
     * @return The number of frames per second
     */
    double AsDecimal() const;
    /**
    * Convert the specified frame number to a floating-point number of seconds based on this framerate
    * @param FrameTime         The frame number to convert
    * @return The number of seconds that the specified frame number represents
    */
    double AsSeconds(FFrameTime FrameTime) const;

    void Serialize(FArchive& Ar) const
    {
        Ar << Numerator << Denominator;
    }

    void Deserialize(FArchive& Ar)
    {
        Ar >> Numerator >> Denominator;
    }
};

inline double FFrameRate::AsInterval() const
{
    return double(Denominator) / double(Numerator);
}

inline double FFrameRate::AsDecimal() const
{
    return double(Numerator) / double(Denominator);
}

inline double FFrameRate::AsSeconds(FFrameTime FrameTime) const
{
    const int64  IntegerPart  = FrameTime.GetFrame() * int64(Denominator);
    const double FloatPart    = FrameTime.GetSubFrame()    * double(Denominator);

    return (double(IntegerPart) + FloatPart) / Numerator;
}