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

bool operator==(const FAnimNotifyEvent& A, const FAnimNotifyEvent& B)
{
    if (A.TriggerTime == B.TriggerTime && A.Duration == B.Duration && A.NotifyName == B.NotifyName)
        return true;
    return false;
}