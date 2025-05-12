#include "SlateRect.h"

#include "Math/Point.h"
#include "Math/Vector.h"

bool FRect::Contains(const FVector2D Point) const
{
    return Point.X >= LeftTopX && Point.X <= GetRight() && Point.Y >= LeftTopY && Point.Y <= GetBottom();
}

bool FRect::Contains(const FPoint Point) const
{
    return Point.X >= LeftTopX && Point.X <= GetRight() && Point.Y >= LeftTopY && Point.Y <= GetBottom();
}
