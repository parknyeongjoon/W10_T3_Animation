#pragma once
#include "Container/Map.h"
#include "Container/String.h"
#include "Engine/World.h"

class SLevelEditor;
class UEditorPanel;

class UnrealEd
{
public:
    UnrealEd() = default;
    ~UnrealEd() = default;
    void Initialize(SLevelEditor* LevelEditor, float Width, float Height);
    
    void Render() const;
    void RenderInPIE() const;
    void OnResize(HWND hWnd) const;
    void SetWorld(UWorld* InWorld);
    
    void AddEditorPanel(const FString& PanelId, const std::shared_ptr<UEditorPanel>& EditorPanel);
    std::shared_ptr<UEditorPanel> GetEditorPanel(const FString& PanelId);

private:
    TMap<FString, std::shared_ptr<UEditorPanel>> Panels;
    UWorld* World = nullptr;
};
