#pragma once

struct FPoint;
struct FVector2D;

// TODO FSlateRect 변경
class FRect
{
public:
    FRect() : LeftTopX(0), LeftTopY(0), Width(0), Height(0) {}
    explicit FRect(const float X, const float Y, const float W, const float H) : LeftTopX(X), LeftTopY(Y), Width(W), Height(H) {}
    float LeftTopX, LeftTopY, Width, Height;

    bool Contains(FVector2D Point) const;
    
    bool Contains(FPoint Point) const;

    float GetRight() const { return LeftTopX + Width; }
    float GetBottom() const { return LeftTopY + Height; }
};
