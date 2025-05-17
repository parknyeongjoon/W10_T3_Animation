#include "ParticlesEmitterPanel.h"
#include "Classes/Particles/ParticleEmitter.h"
#include "Classes/Particles/ParticleModuleColor.h"
#include "Classes/Particles/ParticleModuleLifetime.h"
#include "Classes/Particles/ParticleModuleLocation.h"
#include "Classes/Particles/ParticleModuleRequired.h"
#include "Classes/Particles/ParticleModuleSize.h"
#include "Classes/Particles/ParticleModuleSpawn.h"
#include "Classes/Particles/ParticleModuleVelocity.h"


void ParticlesEmitterPanel::Initialize(SLevelEditor* LevelEditor, float InWidth, float InHeight)
{
    activeLevelEditor = LevelEditor;
    Width = InWidth;
    Height = InHeight;
}

void ParticlesEmitterPanel::Render()
{
    /* Pre Setup */
    ImGuiIO& io = ImGui::GetIO();

    const float PanelWidth = (Width) * 0.6f;
    const float PanelHeight = (Height) * 0.6f - 20.0f;

    const float PanelPosX = Width - PanelWidth;
    const float PanelPosY = 20.0f;

    /* Panel Position */
    ImGui::SetNextWindowPos(ImVec2(PanelPosX, PanelPosY), ImGuiCond_Always);

    /* Panel Size */
    ImGui::SetNextWindowSize(ImVec2(PanelWidth, PanelHeight), ImGuiCond_Always);

    ImGui::Begin("Emitters", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
    ImGui::End();
    ImGui::ShowDemoWindow();
}

void ParticlesEmitterPanel::OnResize(HWND hWnd)
{
    RECT ClientRect;
    GetClientRect(hWnd, &ClientRect);
    Width = static_cast<float>(ClientRect.right - ClientRect.left);
    Height = static_cast<float>(ClientRect.bottom - ClientRect.top);
}

void ParticlesEmitterPanel::RenderEmitter(UParticleEmitter* Emitter) const
{
}

void ParticlesEmitterPanel::RenderModule(UParticleModue) const
{
}
