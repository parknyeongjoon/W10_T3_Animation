#pragma once
#include "Container/Map.h"
#include "Container/String.h"
#include "ContentsPanel.h"
#include "Engine/World.h"

class FContentsUI
{
public:
    FContentsUI() = default;
    ~FContentsUI() = default;
    void Initialize();
    
    void Render() const;
    void OnResize(HWND hWnd) const;

    void SetWorld(UWorld* InWorld);
    
    void AddEditorPanel(const FString& PanelId, const std::shared_ptr<FContentsPanel>& EditorPanel);
    std::shared_ptr<FContentsPanel> GetEditorPanel(const FString& PanelId);

private:
    TMap<FString, std::shared_ptr<FContentsPanel>> Panels;
    UWorld* World = nullptr;
};
