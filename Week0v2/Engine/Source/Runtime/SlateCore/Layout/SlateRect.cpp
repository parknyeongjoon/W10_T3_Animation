#include "SlateRect.h"

#include "Math/Point.h"
#include "Math/Vector.h"

bool FRect::Contains(FVector2D Point) const
{
    return Point.X >= LeftTopX && Point.X <= GetRight() && Point.Y >= LeftTopY && Point.Y <= GetBottom();
}

bool FRect::Contains(FPoint Point) const
{
    return Point.X >= LeftTopX && Point.X <= GetRight() && Point.Y >= LeftTopY && Point.Y <= GetBottom();
}
