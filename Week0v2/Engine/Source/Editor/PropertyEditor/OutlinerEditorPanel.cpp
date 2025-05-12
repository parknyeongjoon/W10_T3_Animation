#include "OutlinerEditorPanel.h"
#include "EditorEngine.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "ImGUI/imgui.h"

extern UEngine* GEngine;

void OutlinerEditorPanel::Initialize(float InWidth, float InHeight)
{
    Width = InWidth;
    Height = InHeight;

    static int Index = 0;
    PanelIndex = Index++;
}

void OutlinerEditorPanel::Render()
{
    ImGui::PushID(PanelIndex);
    /* Pre Setup */
    ImGuiIO& io = ImGui::GetIO();
    
    float PanelWidth = (Width) * 0.2f - 6.0f;
    float PanelHeight = (Height) * 0.3f;

    float PanelPosX = (Width) * 0.8f + 5.0f;
    float PanelPosY = 5.0f;

    ImVec2 MinSize(140, 100);
    ImVec2 MaxSize(FLT_MAX, 500);
    
    /* Min, Max Size */
    ImGui::SetNextWindowSizeConstraints(MinSize, MaxSize);
    
    /* Panel Position */
    ImGui::SetNextWindowPos(ImVec2(PanelPosX, PanelPosY), ImGuiCond_Always);

    /* Panel Size */
    ImGui::SetNextWindowSize(ImVec2(PanelWidth, PanelHeight), ImGuiCond_Always);

    /* Panel Flags */
    ImGuiWindowFlags PanelFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;
    
    /* Render Start */
    ImGui::Begin("Outliner", nullptr, PanelFlags);

    /*
    if (ImGui::TreeNode("Actors")) // 트리 노드 생성
    {
        for (AActor* Actor : World->GetActors())
        {
            TSet<AActor*> Actros = World->GetSelectedActors();
            bool bSelected = !Actros.IsEmpty() && *Actros.begin() == Actor;
            if (ImGui::Selectable(*Actor->GetActorLabel(), bSelected))
            {
                World->SetSelectedActor(Actor);
                break;
            }
        }
        ImGui::TreePop(); // 트리 닫기
    }
    */
    FString WorldName = World->GetName();
    ImGuiTreeNodeFlags WorldTreeNodeFlags = ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
    if (ImGui::TreeNodeEx(*WorldName, WorldTreeNodeFlags)) // 트리 노드 생성
    {
        for (AActor* Actor : World->GetActors())
        {
            TSet<AActor*> Actros = World->GetSelectedActors();
            bool bSelected = !Actros.IsEmpty() && *Actros.begin() == Actor;
            if (ImGui::Selectable(*Actor->GetActorLabel(), bSelected))
            {
                World->SetSelectedActor(Actor);
                break;
            }
        }
        ImGui::TreePop(); // 트리 닫기
    }

    ImGui::End();
    ImGui::PopID();
}
    
void OutlinerEditorPanel::OnResize(HWND hWnd)
{
    RECT clientRect;
    GetClientRect(hWnd, &clientRect);
    Width = clientRect.right - clientRect.left;
    Height = clientRect.bottom - clientRect.top;
}
