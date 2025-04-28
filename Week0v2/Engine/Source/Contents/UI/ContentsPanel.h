#pragma once
#include "HAL/PlatformType.h"


class FContentsPanel
{
public:
    virtual ~FContentsPanel() = default;
    virtual void Render() = 0;
    virtual void OnResize(HWND hWnd);

private:
    
    LONG Width = 0, Height = 0;
};
