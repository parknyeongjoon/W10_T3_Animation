#pragma once

struct FPoint
{
    FPoint() : X(0), Y(0) {}
    explicit FPoint(float _x, float _y) : X(_x), Y(_y) {}
    explicit FPoint(long _x, long _y) : X(_x), Y(_y) {}
    explicit FPoint(int _x, int _y) : X(_x), Y(_y) {}

    float X, Y;
};
