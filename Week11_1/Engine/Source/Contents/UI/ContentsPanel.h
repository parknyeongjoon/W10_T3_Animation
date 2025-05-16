#pragma once
#include "Engine/World.h"


class FContentsPanel
{
public:
    virtual ~FContentsPanel() = default;
    virtual void Render() = 0;
    virtual void OnResize(HWND hWnd);
    void SetWorld(UWorld* InWorld);

private:
    
    LONG Width = 0, Height = 0;

    UWorld* World = nullptr;
};
