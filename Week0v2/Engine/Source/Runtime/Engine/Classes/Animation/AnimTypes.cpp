#include "AnimTypes.h"

void FRawAnimSequenceTrack::Serialize(FArchive& Ar) const
{
    Ar << PosKeys << RotKeys << ScaleKeys << KeyTimes << static_cast<uint8>(InterpMode);
}

void FRawAnimSequenceTrack::Deserialize(FArchive& Ar)
{
    uint8 interpMode;
    Ar >> PosKeys >> RotKeys >> ScaleKeys >> KeyTimes >> interpMode;
    InterpMode = static_cast<EAnimInterpolationType>(interpMode);
}
