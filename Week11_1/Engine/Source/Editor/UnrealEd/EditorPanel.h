#pragma once

#ifndef __ICON_FONT_INDEX__

#define __ICON_FONT_INDEX__
#define DEFAULT_FONT		0
#define	FEATHER_FONT		1

#endif // !__ICON_FONT_INDEX__

#include "Engine/World.h"

class UEditorPanel
{
public:
    virtual ~UEditorPanel() = default;
    virtual void Render() = 0;
    virtual void OnResize(HWND hWnd) = 0;
    void SetWorld(UWorld* InWorld) { World = InWorld; }

    UWorld* World = nullptr;

    uint32 PanelIndex = 0;

    bool bIsVisible = true;
};
