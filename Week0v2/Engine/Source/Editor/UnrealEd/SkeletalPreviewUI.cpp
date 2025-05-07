#include "SkeletalPreviewUI.h"
#include "EditorPanel.h"

#include "PropertyEditor/ControlEditorPanel.h"
#include "PropertyEditor/OutlinerEditorPanel.h"
#include "PropertyEditor/PreviewControlEditorPanel.h"
#include "PropertyEditor/PrimitiveDrawEditor.h"
#include "PropertyEditor/PropertyEditorPanel.h"
#include "PropertyEditor/SkeletalPreviewPropertyEditorPanel.h"

void FSkeletalPreviewUI::Initialize(SLevelEditor* LevelEditor, float Width, float Height)
{
    auto ControlPanel = std::make_shared<PreviewControlEditorPanel>();
    ControlPanel->Initialize(LevelEditor, Width, Height);
    Panels["PreviewControlPanel"] = ControlPanel;
    
    auto OutlinerPanel = std::make_shared<OutlinerEditorPanel>();
    OutlinerPanel->Initialize(Width, Height);
    Panels["OutlinerPanel"] = OutlinerPanel;
    
    auto PropertyPanel = std::make_shared<SkeletalPreviewPropertyEditorPanel>();
    PropertyPanel->Initialize(Width, Height);   
    Panels["PropertyPanel"] = PropertyPanel;
    
    auto PrimitiveDrawer = std::make_shared<PrimitiveDrawEditor>();
    Panels["PrimitiveDrawEditor"] = PrimitiveDrawer;
}

void FSkeletalPreviewUI::Render() const
{
    for (const auto& Panel : Panels)
    {
        Panel.Value->Render();
    }
}

void FSkeletalPreviewUI::AddEditorPanel(const FString& PanelId, const std::shared_ptr<UEditorPanel>& EditorPanel)
{
    Panels[PanelId] = EditorPanel;
}

void FSkeletalPreviewUI::OnResize(HWND hWnd) const
{
    for (auto& Panel : Panels)
    {
        Panel.Value->OnResize(hWnd);
    }
}

void FSkeletalPreviewUI::SetWorld(UWorld* InWorld)
{
    World = InWorld;
    for (auto& [_, Panel] : Panels)
    {
        Panel->SetWorld(World);
    } 
}

std::shared_ptr<UEditorPanel> FSkeletalPreviewUI::GetEditorPanel(const FString& PanelId)
{
    return Panels[PanelId];
}
