#include "Viewport.h"

#include "SlateCore/Layout/SlateRect.h"

FViewport::FViewport()
{
    Viewport.MaxDepth = 1.0f;
    Viewport.MinDepth = 0.0f;
}

void FViewport::ResizeViewport(FRect Top, FRect Bottom, FRect Left, FRect Right)
{
    switch (ViewScreenLocation)
    {
    case EViewScreenLocation::TopLeft:
        Viewport.TopLeftX = Left.LeftTopX;
        Viewport.TopLeftY = Top.LeftTopY;
        Viewport.Width = Left.Width;
        Viewport.Height = Top.Height;
        break;
    case EViewScreenLocation::TopRight:
        Viewport.TopLeftX = Right.LeftTopX;
        Viewport.TopLeftY = Top.LeftTopY;
        Viewport.Width = Right.Width;
        Viewport.Height = Top.Height;
        break;
    case EViewScreenLocation::BottomLeft:
        Viewport.TopLeftX = Left.LeftTopX;
        Viewport.TopLeftY = Bottom.LeftTopY;
        Viewport.Width = Left.Width;
        Viewport.Height = Bottom.Height;
        break;
    case EViewScreenLocation::BottomRight:
        Viewport.TopLeftX = Right.LeftTopX;
        Viewport.TopLeftY = Bottom.LeftTopY;
        Viewport.Width = Right.Width;
        Viewport.Height = Bottom.Height;
        break;
    default:
        break;
    }
}

void FViewport::ResizeViewport(FRect NewRect)
{
    Viewport.TopLeftX = NewRect.LeftTopX;
    Viewport.TopLeftY = NewRect.LeftTopY;
    Viewport.Width = NewRect.Width;
    Viewport.Height = NewRect.Height;
}

FRect FViewport::GetFSlateRect() const
{
    return FRect(Viewport.TopLeftX, Viewport.TopLeftY, Viewport.Width, Viewport.Height);
}

