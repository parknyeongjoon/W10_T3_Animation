#pragma once
#include "IWindowToggleable.h"
#include "UnrealEd/EditorViewportClient.h"
class ViewportTypePanel : public IWindowToggleable
{
private:
    ViewportTypePanel();
public:
    ~ViewportTypePanel();

    static ViewportTypePanel& GetInstance();

    void Draw(std::shared_ptr<FEditorViewportClient> ActiveViewport);
    void OnResize(HWND hWnd);
    void Toggle() override {
        if (bWasOpen) {
            bWasOpen = false;
        }
    }

private:
    bool bWasOpen = true;
    UINT width;
    UINT height;
};

