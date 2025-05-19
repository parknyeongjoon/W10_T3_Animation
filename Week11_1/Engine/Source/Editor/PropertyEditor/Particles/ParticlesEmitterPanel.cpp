#include "ParticlesEmitterPanel.h"
#include "CoreUObject/UObject/Casts.h"
#include "UnrealEd/ParticlePreviewUI.h"

#include "Classes/Particles/ParticleEmitter.h"
#include "Classes/Particles/ParticleModuleColor.h"
#include "Classes/Particles/ParticleModuleLifetime.h"
#include "Classes/Particles/ParticleModuleLocation.h"
#include "Classes/Particles/ParticleModuleRequired.h"
#include "Classes/Particles/ParticleModuleSize.h"
#include "Classes/Particles/ParticleModuleSpawn.h"
#include "Classes/Particles/ParticleModuleVelocity.h"
#include "Classes/Particles/ParticleLODLevel.h"

void ParticlesEmitterPanel::Initialize(SLevelEditor* LevelEditor, float InWidth, float InHeight)
{
    activeLevelEditor = LevelEditor;
    Width = InWidth;
    Height = InHeight;

    UClass* ModuleClass = UClass::FindClass("UParticleModule");
    GetChildOfClass(ModuleClass, ModuleClasses);
    ModuleClasses.Remove(UClass::FindClass("UParticleModule"));
    ModuleClasses.Remove(UClass::FindClass("UParticleModuleRequired"));
    ModuleClasses.Remove(UClass::FindClass("UParticleModuleSpawn"));
}

void ParticlesEmitterPanel::Render()
{
    /* Pre Setup */
    ImGuiIO& io = ImGui::GetIO();

    const float PanelWidth = (Width) * (1 - UI->PreviewScreenWidth);
    const float PanelHeight = (Height) * (1 - UI->PreviewScreenHeight) - 20.0f;

    const float PanelPosX = Width - PanelWidth;
    const float PanelPosY = 20.0f;

    /* Panel Position */
    ImGui::SetNextWindowPos(ImVec2(PanelPosX, PanelPosY), ImGuiCond_Always);

    /* Panel Size */
    ImGui::SetNextWindowSize(ImVec2(PanelWidth, PanelHeight), ImGuiCond_Always);

    ImGui::Begin("Emitters Panel", nullptr);

    // 예시: 여러 Emitter
    // UParticleEmitter로 변경할것
    const TArray<UParticleEmitter*>& Emitters = UI->GetSelectedSystem()->Emitters;

    // Module을 정렬
    struct EmitterModulesSorted {
        UParticleEmitter* Emitter;
        TArray<UParticleModule*> ModulesSorted;
    };

    TArray<EmitterModulesSorted> EmitterModulesSortedArray;

    static const int RequiredIndex = 0;
    static const int SpawnIndex = 1;

    // COLUMNS
    for (UParticleEmitter* Emitter : Emitters)
    {
        // 1. Module 정렬
        TArray<UParticleModule*> Modules = Emitter->LODLevels[UI->GetSelectedLODIndex()]->Modules;

        // Required 찾기
        // iterate중에 바꿔도 되나??
        for (int idx = RequiredIndex; idx < Modules.Num(); ++idx)
        {
            UParticleModule* Module = Modules[idx];
            if (Module && Module->IsA<UParticleModuleRequired>())
            {
                if (idx != RequiredIndex)
                {
                    UParticleModule* Temp = Modules[RequiredIndex];
                    Modules[RequiredIndex] = Module;
                    Modules[idx] = Temp;
                }
                break;
            }
        }

        // Spawn 찾기
        for (int idx = SpawnIndex; idx < Modules.Num(); ++idx)
        {
            UParticleModule* Module = Modules[idx];
            if (Module && Module->IsA<UParticleModuleSpawn>())
            {
                if (idx != SpawnIndex)
                {
                    UParticleModule* Temp = Modules[SpawnIndex];
                    Modules[SpawnIndex] = Module;
                    Modules[idx] = Temp;
                }
                break;
            }
        }

        EmitterModulesSortedArray.Add({ Emitter, Modules });
    }

    // 3. 테이블 시작
    if (ImGui::BeginTable("EmitterModules", EmitterModulesSortedArray.Num(), ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    {
        int MaxNumModules = 0;
        for (const EmitterModulesSorted& EmitterModules : EmitterModulesSortedArray)
        {
            MaxNumModules = FMath::Max(MaxNumModules, EmitterModules.ModulesSorted.Num());
        }

        for (const EmitterModulesSorted& EmitterModules : EmitterModulesSortedArray)
        {
            const UParticleEmitter* Emitter = EmitterModules.Emitter;
            ImGui::TableSetupColumn((*Emitter->GetName()));
        }
        ImGui::TableHeadersRow();

        // + 버튼을 위해서 최대개수보다 하나 더 추가
        for (int row = 0; row < MaxNumModules + 1; ++row)
        {
            ImGui::TableNextRow();
            for (int col = 0; col < EmitterModulesSortedArray.Num(); ++col)
            {
                ImGui::TableSetColumnIndex(col);
                UParticleEmitter* Emitter = EmitterModulesSortedArray[col].Emitter;
                if (row < EmitterModulesSortedArray[col].ModulesSorted.Num())
                {
                    UParticleModule* Module = EmitterModulesSortedArray[col].ModulesSorted[row];
                    RenderModuleCell(Module, Emitter);
                }
                else if (row == EmitterModulesSortedArray[col].ModulesSorted.Num())
                {
                    RenderModuleAdd(Emitter);
                }
                else
                {
                    RenderModuleCell(nullptr, nullptr);
                }
            }
        }
        ImGui::EndTable();
    }
    ImGui::End();
}

void ParticlesEmitterPanel::RenderModuleCell(UParticleModule* Module, UParticleEmitter* Emitter) const
{
    if (!Module) return;
    
    bool Checked = UI->IsEnabled(Module);
    FString ModuleName = Module->GetClass()->GetName();
    ModuleName.RemoveFromStart(TEXT("UParticleModule"));
    ImGui::Checkbox(*FString::Printf("##%d", Module->GetUUID()), &Checked);
    // 체크했을때의 action은 없음
    UI->SetEnabled(Module, Checked);

    ImGui::SameLine();
    
    if (ImGui::Button(*FString::Printf("%s##%d", *ModuleName, Module->GetUUID())))
    {
        UI->SetSelectedModule(Module);
    }

    // 버튼 우클릭시 제거
    if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
    {
        if (!Module->IsA<UParticleModuleRequired>() && !Module->IsA<UParticleModuleSpawn>())
        {
            for (int i = 0; i < Emitter->LODLevels.Num(); ++i)
            {
                Emitter->LODLevels[i]->Modules.Remove(Module);
            }
            UI->RemoveFlags(Module);
            GUObjectArray.MarkRemoveObject(Module);
        }
    }
}

void ParticlesEmitterPanel::RenderModuleAdd(UParticleEmitter* Emitter)
{
    uint32 Identifier = Emitter->GetUUID();

    // Popup ID 고정 문자열 생성
    FString PopupIdStr = FString::Printf(TEXT("AddModulePopup##%u"), Identifier);
    const char* PopupId = *PopupIdStr;

    // 버튼
    if (ImGui::Button(*FString::Printf(TEXT("Add Module##%u_AddEmitter"), Identifier)))
    {
        ImGui::OpenPopup(PopupId);
    }

    // 팝업 시작
    if (ImGui::BeginPopup(PopupId))
    {
        for (UClass* Child : ModuleClasses)
        {
            if (ImGui::MenuItem(*Child->GetName()))
            {
                // 모든 LOD level에 추가
                for (int l = 0; l < Emitter->LODLevels.Num(); ++l)
                {
                    UParticleModule* NewModule = Cast<UParticleModule>(FObjectFactory::ConstructObject(Child, Emitter));
                    Emitter->LODLevels[l]->Modules.Add(NewModule);
                    UI->RegisterFlags(NewModule);
                }
                ImGui::CloseCurrentPopup();
            }
        }

        ImGui::EndPopup();
    }
}



void ParticlesEmitterPanel::OnResize(HWND hWnd)
{
    RECT ClientRect;
    GetClientRect(hWnd, &ClientRect);
    Width = static_cast<float>(ClientRect.right - ClientRect.left);
    Height = static_cast<float>(ClientRect.bottom - ClientRect.top);
}