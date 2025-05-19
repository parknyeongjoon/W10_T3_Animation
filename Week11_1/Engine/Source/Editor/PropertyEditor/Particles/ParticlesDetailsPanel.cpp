#include "ParticlesDetailsPanel.h"
#include "UnrealEd/ParticlePreviewUI.h"
#include "Classes/Particles/ParticleEmitter.h"
#include "Classes/Particles/Modules/ParticleModule.h"

void ParticlesDetailsPanel::Initialize(SLevelEditor* LevelEditor, float InWidth, float InHeight)
{
    activeLevelEditor = LevelEditor;
    Width = InWidth;
    Height = InHeight;
}

void ParticlesDetailsPanel::Render()
{
    /* Pre Setup */
    ImGuiIO& io = ImGui::GetIO();

    const float PanelWidth = (Width) * UI->PreviewScreenWidth;
    const float PanelHeight = (Height) * UI->PreviewScreenHeight;

    const float PanelPosX = 0.f;
    const float PanelPosY = Height - PanelHeight;

    /* Panel Position */
    ImGui::SetNextWindowPos(ImVec2(PanelPosX, PanelPosY), ImGuiCond_Always);

    /* Panel Size */
    ImGui::SetNextWindowSize(ImVec2(PanelWidth, PanelHeight), ImGuiCond_Always);

    TArray<UParticleModule*> Modules = UI->GetSelectedLODLevel()->Modules;
    
    ImGui::Begin("Details", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

    int SelectedModuleIndex = UI->GetSelectedModuleIndex();
    if (SelectedModuleIndex >= 0 && SelectedModuleIndex < Modules.Num())
    {
        if (UParticleModule* SelectedModule = Modules[SelectedModuleIndex])
        {
            const UClass* Class = SelectedModule->GetClass();
            //for (; Class; Class = Class->GetSuperClass())
            //{
                const TArray<FProperty*>& Properties = Class->GetProperties();
                if (!Properties.IsEmpty())
                {
                    ImGui::SeparatorText(*Class->GetName());
                }

                for (const FProperty* Prop : Properties)
                {
                    Prop->DisplayInImGui(SelectedModule);
                }
            //}
        }
    }

    ImGui::End();
}

void ParticlesDetailsPanel::OnResize(HWND hWnd)
{
    RECT ClientRect;
    GetClientRect(hWnd, &ClientRect);
    Width = static_cast<float>(ClientRect.right - ClientRect.left);
    Height = static_cast<float>(ClientRect.bottom - ClientRect.top);
}

void ParticlesDetailsPanel::RenderFileMenu() const
{
}

void ParticlesDetailsPanel::RenderEditMenu() const
{
}
