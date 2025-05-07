#pragma once

#include "SlateCore/Layout/SlateRect.h"

struct FPoint;

class SWindow
{
public:
    SWindow();
    SWindow(FRect initRect);
    virtual ~SWindow();

    virtual void Initialize(FRect initRect);
    virtual void Resize(float InWidth, float InHeight);

    virtual bool OnPressed(FPoint coord);
    virtual bool OnReleased();
    
    void SetRect(FRect NewRect) { Rect = NewRect; }
    const FRect& GetRect() const { return Rect; }
    
    bool IsPressed() const { return bIsPressed; }
    bool IsHover(FPoint coord);
    
    
protected:
    bool bIsHovered = false;
    bool bIsPressed = false;
    
    FRect Rect;
};

