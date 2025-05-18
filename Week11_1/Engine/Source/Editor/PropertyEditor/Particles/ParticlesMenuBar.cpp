#include "ParticlesMenuBar.h"
#include "UnrealEd/ParticlePreviewUI.h"

void ParticlesMenuBar::Initialize(SLevelEditor* LevelEditor, float InWidth, float InHeight)
{
    activeLevelEditor = LevelEditor;
    Width = InWidth;
    Height = InHeight;
}

void ParticlesMenuBar::Render()
{
    /* Pre Setup */
    ImGuiIO& io = ImGui::GetIO();
    ImFont* IconFont = io.Fonts->Fonts[FEATHER_FONT];
    ImVec2 IconSize = ImVec2(32, 32);

    float PanelWidth = (Width) * 0.8f;
    float PanelHeight = 45.0f;

    float PanelPosX = 1.0f;
    float PanelPosY = 1.0f;

    //ImVec2 MinSize(300, 50);
    //ImVec2 MaxSize(FLT_MAX, 50);

    ///* Min, Max Size */
    //ImGui::SetNextWindowSizeConstraints(MinSize, MaxSize);

    ///* Panel Position */
    //ImGui::SetNextWindowPos(ImVec2(PanelPosX, PanelPosY), ImGuiCond_Always);

    ///* Panel Size */
    //ImGui::SetNextWindowSize(ImVec2(PanelWidth, PanelHeight), ImGuiCond_Always);

    ///* Panel Flags */
    //ImGuiWindowFlags PanelFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground;

    /* Render Start */

    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open")) {}
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo"))
            {

            }
            if (ImGui::MenuItem("Redo"))
            {

            }
            ImGui::EndMenu();
        }
        ImGui::SameLine();
        ImGui::Dummy(ImVec2(50, 0));
        static ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_Reorderable;
        tab_bar_flags |= ImGuiTabBarFlags_NoCloseWithMiddleMouseButton;
        tab_bar_flags |= ImGuiTabBarFlags_FittingPolicyScroll;

        //static TArray<FString> Particles;
        //static int num = 0;
        //char buf[8];

        const TArray<FParticleEmitterInstance*> ParticleEmitterInstances = ParticlePreviewUI->GetParticleEmitterInstances();

        if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
        {
            if (ImGui::TabItemButton("+", ImGuiTabItemFlags_Trailing | ImGuiTabItemFlags_NoTooltip))
            {
                //_itoa_s(num++, buf, 10);
                //Particles.Add(FString(buf)); // Add new tab
                ParticlePreviewUI->CreateEmptyParticleEmitterInstance();
            }

            for (int n = 0; n < ParticleEmitterInstances.Num(); n++)
            {
                bool open = true;
                FString InstanceName = ParticleEmitterInstances[n]->EmitterName.ToString();
                if (ImGui::BeginTabItem(*InstanceName, &open, ImGuiTabItemFlags_None))
                {
                    ImGui::Text("This is the %s tab!", *InstanceName);
                    ImGui::EndTabItem();
                }
                if (!open)
                {
                    ParticlePreviewUI->RemoveParticleEmitterInstance(n);
                }
            }
            ImGui::EndTabBar();
        }

        ImGui::EndMainMenuBar();
    }

}

void ParticlesMenuBar::OnResize(HWND hWnd)
{
    // 무조건 왼쪽위
    //RECT ClientRect;
    //GetClientRect(hWnd, &ClientRect);
    //Width = static_cast<float>(ClientRect.right - ClientRect.left);
    //Height = static_cast<float>(ClientRect.bottom - ClientRect.top);
}

void ParticlesMenuBar::RenderFileMenu() const
{
}

void ParticlesMenuBar::RenderEditMenu() const
{
}
