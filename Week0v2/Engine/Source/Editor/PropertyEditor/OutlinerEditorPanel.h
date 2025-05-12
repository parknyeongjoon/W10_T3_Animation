#pragma once
#include "UnrealEd/EditorPanel.h"

class OutlinerEditorPanel : public UEditorPanel
{
public:
    OutlinerEditorPanel() = default;

public:
    void Initialize(float InWidth, float InHeight);
    virtual void Render() override;
    virtual void OnResize(HWND hWnd) override;

private:
    float Width = 0, Height = 0;

    /* 시퀀서 용 변수는 이 아래에 작성해 주세요 */
public:
    // ...
};
