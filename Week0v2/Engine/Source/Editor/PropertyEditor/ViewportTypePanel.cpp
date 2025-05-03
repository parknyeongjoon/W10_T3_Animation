#include "ViewportTypePanel.h"

#include "ImGUI/imgui.h"

ViewportTypePanel::ViewportTypePanel()
{
}

ViewportTypePanel::~ViewportTypePanel()
{
}

ViewportTypePanel& ViewportTypePanel::GetInstance()
{
    static ViewportTypePanel instance;
    return instance;
}


void ViewportTypePanel::Draw(std::shared_ptr<FEditorViewportClient> ActiveViewport)
{
    float controllWindowWidth = static_cast<float>(width) * 0.12f;
    float controllWindowHeight = static_cast<float>(height) * 0.f;

    float controllWindowPosX = (static_cast<float>(width) - controllWindowWidth) * 0.5f;
    float controllWindowPosY = (static_cast<float>(height) - controllWindowHeight) * 0.f;
    ImGui::SetNextWindowPos(ImVec2(controllWindowPosX, controllWindowPosY));
    ImGui::SetNextWindowSize(ImVec2(controllWindowWidth, controllWindowHeight), ImGuiCond_Always);

    if (ImGui::Begin("ViewportType"))
    {
        const char* cameraModeNames[] = { "Top", "Front", "Left","Perspective", "Bottom", "Back", "Right"};

        if (ImGui::BeginCombo("##Viewport Type", cameraModeNames[(int)ActiveViewport->GetViewportType()]))
        {
            for (int i = 0; i < IM_ARRAYSIZE(cameraModeNames); i++)
            {
                bool isSelected = ((int)ActiveViewport->GetViewMode() == i);
                if (ImGui::Selectable(cameraModeNames[i], isSelected))
                {
                    ActiveViewport->SetViewportType((ELevelViewportType)i);

                }

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
    }
    ImGui::End();
}

void ViewportTypePanel::OnResize(HWND hWnd)
{
    RECT clientRect;
    GetClientRect(hWnd, &clientRect);
    width = clientRect.right - clientRect.left;
    height = clientRect.bottom - clientRect.top;
}
