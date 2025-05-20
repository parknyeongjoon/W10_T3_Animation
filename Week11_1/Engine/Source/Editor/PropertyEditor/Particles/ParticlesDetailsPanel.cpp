#include "ParticlesDetailsPanel.h"
#include "CoreUObject/UObject/Casts.h"
#include "UnrealEd/ParticlePreviewUI.h"
#include "Classes/Particles/ParticleEmitter.h"
#include "Classes/Particles/Modules/ParticleModule.h"
#include "Classes/Distributions/DistributionFloat.h"
#include "Classes/Distributions/DistributionVector.h"

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
            const TArray<FProperty*>& Properties = Class->GetProperties();
            
            // distribution관련 지우기
            // 그리고 distribution인건 따로 그리기
            // distribution일 경우 float 수정할수있게 따로 그리기
            //Properties.Remove()
            if (!Properties.IsEmpty())
            {
                ImGui::SeparatorText(*Class->GetName());
            }

            for (const FProperty* Prop : Properties)
            {
                UScriptStruct* Struct = std::get<UScriptStruct*>(Prop->TypeSpecificData);
                if (Struct->IsChildOf(FRawDistributionFloat::StaticStruct()))
                {
                    ImGui::SeparatorText(Prop->Name);
                    FRawDistributionFloat* Distribution = static_cast<FRawDistributionFloat*>(Prop->GetPropertyData(SelectedModule));
                    RenderDistributionMenu(Distribution, true);
                    continue;
                }
                else if (Struct->IsChildOf(FRawDistributionVector::StaticStruct()))
                {
                    ImGui::SeparatorText(Prop->Name);
                    FRawDistributionVector* Distribution = static_cast<FRawDistributionVector*>(Prop->GetPropertyData(SelectedModule));
                    RenderDistributionMenu(Distribution, false);
                    continue;
                }
                Prop->DisplayInImGui(SelectedModule);
            }
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

void ParticlesDetailsPanel::RenderDistributionMenu(FRawDistribution* Distribution, bool bFloat)
{
    TArray<UClass*> CandidateClasses;
    UClass* BaseClass = nullptr;
    UDistribution* CurrentDistribution = nullptr;
    
    // 현재 Distribution 타입 확인
    // 내부에서 float 찾는 방법을 찾지 못함...
    if (bFloat)
    {
        CurrentDistribution = static_cast<FRawDistributionFloat*>(Distribution)->Distribution;
        BaseClass = UDistributionFloat::StaticClass();
    }
    else // Vector
    {
        CurrentDistribution = static_cast<FRawDistributionVector*>(Distribution)->Distribution;
        BaseClass = UDistributionVector::StaticClass();
    }

    if (!BaseClass)
    {
        ImGui::Text("Unsupported distribution type");
        return;
    }

    // 하위 클래스 수집 (캐싱해도 좋음)
    GetChildOfClass(BaseClass, CandidateClasses);
    CandidateClasses.Remove(BaseClass); // 기본 클래스 제거

    // 현재 선택된 클래스 이름
    FString CurrentName = CurrentDistribution ? CurrentDistribution->GetClass()->GetName() : TEXT("None");
    CurrentName = FString::Printf("%s##%llu", *CurrentName, Distribution);
    // Combo 시작
    ImGui::PushID(Distribution);
    if (ImGui::BeginCombo("Distribution Type", *CurrentName))
    {
        for (UClass* OptionClass : CandidateClasses)
        {
            FString OptionName = OptionClass->GetName();
            OptionName = FString::Printf("%s##%llu", *OptionName, Distribution);

            bool bIsSelected = (CurrentDistribution && CurrentDistribution->GetClass() == OptionClass);

            if (ImGui::Selectable(*OptionName, bIsSelected))
            {
                // 선택이 바뀐 경우: 기존 오브젝트 제거
                if (CurrentDistribution)
                {
                    GUObjectArray.MarkRemoveObject(CurrentDistribution);
                    CurrentDistribution = nullptr;
                }

                // 새 오브젝트 생성
                UDistribution* NewDistribution = Cast<UDistribution>(
                    FObjectFactory::ConstructObject(OptionClass, nullptr)
                );

                if (bFloat)
                {
                    static_cast<FRawDistributionFloat*>(Distribution)->Distribution
                        = Cast<UDistributionFloat>(NewDistribution);
                }
                else
                {
                    static_cast<FRawDistributionVector*>(Distribution)->Distribution
                        = Cast<UDistributionVector>(NewDistribution);
                }
            }
        }
        ImGui::EndCombo();
    }

    // 값 조절 UI
    if (CurrentDistribution)
    {
        for (const FProperty* Prop : CurrentDistribution->GetClass()->GetProperties())
        {
            Prop->DisplayInImGui(CurrentDistribution);
        }

    }
    ImGui::PopID();
}