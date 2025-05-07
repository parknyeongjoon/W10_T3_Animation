#pragma once
#include "Container/Array.h"
#include "ImGUI/imgui.h"
#include "UnrealEd/EditorPanel.h"

struct FDrawRectInfo
{
    ImVec2 topLeft;
    ImVec2 bottomRight;
    ImU32 color;
    float rounding;
    ImDrawFlags drawFlags;
    float thickness;
};
class PrimitiveDrawEditor : public UEditorPanel
{
public:
    virtual void Render() override;
    virtual void OnResize(HWND hWnd) override;

private:
    void ClearRenderInfo();
public:
    TArray<FDrawRectInfo> DrawRectInfo;
};
