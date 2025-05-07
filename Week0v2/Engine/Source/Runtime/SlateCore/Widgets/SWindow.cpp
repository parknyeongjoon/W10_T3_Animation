#include "SWindow.h"

#include "Math/Point.h"

SWindow::SWindow()
{
}

SWindow::SWindow(FRect initRect) : Rect(initRect)
{
}

SWindow::~SWindow()
{
}

void SWindow::Initialize(FRect initRect)
{
    Rect = initRect;
}

void SWindow::Resize(float InWidth, float InHeight)
{
    Rect.Width = InWidth;
    Rect.Height = InHeight;
}

bool SWindow::IsHover(FPoint coord) 
{
    bIsHovered = Rect.Contains(coord);
    return bIsHovered;
}

bool SWindow::OnPressed(FPoint coord)
{
    return false;
}

bool SWindow::OnReleased() 
{
    return false;
}
