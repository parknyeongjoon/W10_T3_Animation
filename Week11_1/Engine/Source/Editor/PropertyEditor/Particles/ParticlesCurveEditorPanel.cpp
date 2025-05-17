#include "ParticlesCurveEditorPanel.h"

void ParticlesCurveEditorPanel::Initialize(SLevelEditor* LevelEditor, float InWidth, float InHeight)
{
    activeLevelEditor = LevelEditor;
    Width = InWidth;
    Height = InHeight;
}

void ParticlesCurveEditorPanel::Render()
{
    ImGui::PushID(PanelIndex);
    /* Pre Setup */
    ImGuiIO& io = ImGui::GetIO();

    const float PanelWidth = (Width) * 0.6f;
    const float PanelHeight = (Height) * 0.4f;

    const float PanelPosX = Width - PanelWidth;
    const float PanelPosY = Height - PanelHeight;

    /* Panel Position */
    ImGui::SetNextWindowPos(ImVec2(PanelPosX, PanelPosY), ImGuiCond_Always);

    /* Panel Size */
    ImGui::SetNextWindowSize(ImVec2(PanelWidth, PanelHeight), ImGuiCond_Always);

    ImGui::Begin("Curves", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
    ImGui::End();
    ImGui::ShowDemoWindow();
    ImGui::PopID();
}

void ParticlesCurveEditorPanel::OnResize(HWND hWnd)
{
    RECT ClientRect;
    GetClientRect(hWnd, &ClientRect);
    Width = static_cast<float>(ClientRect.right - ClientRect.left);
    Height = static_cast<float>(ClientRect.bottom - ClientRect.top);
}

void ParticlesCurveEditorPanel::RenderFileMenu() const
{
}

void ParticlesCurveEditorPanel::RenderEditMenu() const
{
}
