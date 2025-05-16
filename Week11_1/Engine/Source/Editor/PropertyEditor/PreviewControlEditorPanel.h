#pragma once
#include "HAL/PlatformType.h"
#include "UnrealEd/EditorPanel.h"

struct ImFont;
struct ImVec2;
class SLevelEditor;

class PreviewControlEditorPanel : public UEditorPanel
{
public:
    void Initialize(SLevelEditor* levelEditor, float Width, float Height);
    virtual void Render() override;
    virtual void OnResize(HWND hWnd) override;


private:
    void CreateMenuButton(ImVec2 ButtonSize, ImFont* IconFont);
    void CreateModifyButton(ImVec2 ButtonSize, ImFont* IconFont);
    void CreateFlagButton() const;
    void CreateShaderHotReloadButton(ImVec2 ButtonSize) const;
    void CreatePIEButton(ImVec2 ButtonSize) const;
    void CreateSRTButton(ImVec2 ButtonSize) const;

    uint64 ConvertSelectionToFlags(const bool selected[]) const;
    
private:
    SLevelEditor* activeLevelEditor;
    float Width = 300, Height = 100;
    bool bOpenMenu = false;

    float* FOV = nullptr;
    float CameraSpeed = 0.0f;
    float GridScale = 1.0f;  
};

