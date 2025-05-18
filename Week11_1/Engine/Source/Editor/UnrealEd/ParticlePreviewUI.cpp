#include "ParticlePreviewUI.h"
#include "EditorPanel.h"

#include "PropertyEditor/Particles/ParticlesMenuBar.h"
#include "PropertyEditor/Particles/ParticlesDetailsPanel.h"
#include "PropertyEditor/Particles/ParticlesEmitterPanel.h"
#include "PropertyEditor/Particles/ParticlesCurveEditorPanel.h"


void FParticlePreviewUI::Initialize(SLevelEditor* LevelEditor, float Width, float Height)
{
    auto ControlPanel = std::make_shared<ParticlesMenuBar>();
    ControlPanel->Initialize(LevelEditor, Width, Height);
    Panels["PreviewControlPanel"] = ControlPanel;
    ControlPanel->BindUI(this);
    auto DetailsPanel = std::make_shared<ParticlesDetailsPanel>();
    DetailsPanel->Initialize(LevelEditor, Width, Height);
    Panels["DetailsPanel"] = DetailsPanel;
    DetailsPanel->BindUI(this);
    auto EmitterPanel = std::make_shared<ParticlesEmitterPanel>();
    EmitterPanel->Initialize(LevelEditor, Width, Height);
    Panels["EmitterPanel"] = EmitterPanel;
    EmitterPanel->BindUI(this);
    auto CurvePanel = std::make_shared<ParticlesCurveEditorPanel>();
    CurvePanel->Initialize(LevelEditor, Width, Height);
    Panels["CurvePanel"] = CurvePanel;
    CurvePanel->BindUI(this);
}

void FParticlePreviewUI::Render() const
{
    for (const auto& Panel : Panels)
    {
        if (Panel.Value->bIsVisible)
        {
            Panel.Value->Render();
        }
    }

    ImGui::ShowDemoWindow();
}

void FParticlePreviewUI::OnResize(HWND hWnd) const
{
    for (auto& Panel : Panels)
    {
        Panel.Value->OnResize(hWnd);
    }
}

void FParticlePreviewUI::SetWorld(UWorld* InWorld)
{
    World = InWorld;
    for (auto& [_, Panel] : Panels)
    {
        Panel->SetWorld(World);
    }
}

void FParticlePreviewUI::AddEditorPanel(const FString& PanelId, const std::shared_ptr<UEditorPanel>& EditorPanel)
{
    Panels[PanelId] = EditorPanel;
}

std::shared_ptr<UEditorPanel> FParticlePreviewUI::GetEditorPanel(const FString& PanelId)
{
    return Panels[PanelId];
}
