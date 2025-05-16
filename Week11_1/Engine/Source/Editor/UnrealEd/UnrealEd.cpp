#include "UnrealEd.h"
#include "EditorPanel.h"

#include "PropertyEditor/ControlEditorPanel.h"
#include "PropertyEditor/OutlinerEditorPanel.h"
#include "PropertyEditor/PrimitiveDrawEditor.h"
#include "PropertyEditor/PropertyEditorPanel.h"

void UnrealEd::Initialize(SLevelEditor* LevelEditor, float Width, float Height)
{
    auto ControlPanel = std::make_shared<ControlEditorPanel>();
    ControlPanel->Initialize(LevelEditor, Width, Height);
    Panels["ControlPanel"] = ControlPanel;
    
    auto OutlinerPanel = std::make_shared<OutlinerEditorPanel>();
    OutlinerPanel->Initialize(Width, Height);
    Panels["OutlinerPanel"] = OutlinerPanel;
    
    auto PropertyPanel = std::make_shared<PropertyEditorPanel>();
    PropertyPanel->Initialize(Width, Height);   
    Panels["PropertyPanel"] = PropertyPanel;
    
    auto PrimitiveDrawer = std::make_shared<PrimitiveDrawEditor>();
    Panels["PrimitiveDrawEditor"] = PrimitiveDrawer;
}

void UnrealEd::Render() const
{
    for (const auto& Panel : Panels)
    {
        Panel.Value->Render();
    }
}

void UnrealEd::RenderInPIE() const
{
    Panels["ControlPanel"]->Render();
}

void UnrealEd::AddEditorPanel(const FString& PanelId, const std::shared_ptr<UEditorPanel>& EditorPanel)
{
    Panels[PanelId] = EditorPanel;
}

void UnrealEd::OnResize(HWND hWnd) const
{
    for (auto& Panel : Panels)
    {
        Panel.Value->OnResize(hWnd);
    }
}

void UnrealEd::SetWorld(UWorld* InWorld)
{
    World = InWorld;
    for (auto& [_, Panel] : Panels)
    {
        Panel->SetWorld(World);
    } 
}

std::shared_ptr<UEditorPanel> UnrealEd::GetEditorPanel(const FString& PanelId)
{
    return Panels[PanelId];
}
