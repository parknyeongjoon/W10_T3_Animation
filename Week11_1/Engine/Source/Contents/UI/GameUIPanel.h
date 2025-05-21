#pragma once
#include "ContentsUI.h"

class FGameUIPanel : public FContentsPanel
{
public:
    virtual ~FGameUIPanel() override;
    virtual void Render() override;
    virtual void OnResize(HWND hWnd) override;
    
};
