#include "ContentsUI.h"

#include "GameUIPanel.h"

void FContentsUI::Initialize()
{
    auto GameUIPanel = std::make_shared<FGameUIPanel>();
    Panels["GameUIPanel"] = GameUIPanel;
}

void FContentsUI::Render() const
{
    for (const auto& Panel : Panels)
    {
        Panel.Value->Render();
    }
}

void FContentsUI::OnResize(HWND hWnd) const
{
    for (auto& Panel : Panels)
    {
        Panel.Value->OnResize(hWnd);
    }
}

void FContentsUI::SetWorld(UWorld* InWorld)
{
    World = InWorld;
    for (auto& [_, Panel] : Panels)
    {
        Panel->SetWorld(World);
    } 
}

void FContentsUI::AddEditorPanel(const FString& PanelId, const std::shared_ptr<FContentsPanel>& EditorPanel)
{
    Panels[PanelId] = EditorPanel;
}

std::shared_ptr<FContentsPanel> FContentsUI::GetEditorPanel(const FString& PanelId)
{
    return Panels[PanelId];
}
