#pragma once

struct FPoint;
struct FVector2D;

// TODO FSlateRect 변경
class FRect
{
public:
    FRect() : LeftTopX(0), LeftTopY(0), Width(0), Height(0) {}
    explicit FRect(float x, float y, float w, float h) : LeftTopX(x), LeftTopY(y), Width(w), Height(h) {}
    float LeftTopX, LeftTopY, Width, Height;

    bool Contains(FVector2D Point) const;
    
    bool Contains(FPoint Point) const;

    float GetRight() const { return LeftTopX + Width; }
    float GetBottom() const { return LeftTopY + Height; }
};
