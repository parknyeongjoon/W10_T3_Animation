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

    ImGui::Begin("Emitters Panel", nullptr);

    // 예시: 여러 Emitter
    // UParticleEmitter로 변경할것
    TArray<FParticleEmitterInstance*>& Instances = UI->GetParticleEmitterInstances();

    static bool first = true;
    if (first)
    {
        for (int i = 0; i < 5; i++)
        {
            Instances.Add(new FParticleEmitterInstance);
            Instances[i]->SpriteTemplate = FObjectFactory::ConstructObject<UParticleEmitter>(nullptr);
            Instances[i]->SpriteTemplate->LODLevels.Add(FObjectFactory::ConstructObject<UParticleLODLevel>(nullptr));
            Instances[i]->SpriteTemplate->LODLevels[0]->RequiredModule = FObjectFactory::ConstructObject<UParticleModuleRequired>(nullptr);

            Instances[i]->SpriteTemplate->LODLevels[0]->Modules.Add(Instances[i]->SpriteTemplate->LODLevels[0]->RequiredModule);
            Instances[i]->SpriteTemplate->LODLevels[0]->Modules.Add(FObjectFactory::ConstructObject<UParticleModuleColor>(nullptr));
            Instances[i]->SpriteTemplate->LODLevels[0]->Modules.Add(FObjectFactory::ConstructObject<UParticleModuleLifetime>(nullptr));
            Instances[i]->SpriteTemplate->LODLevels[0]->Modules.Add(FObjectFactory::ConstructObject<UParticleModuleLocation>(nullptr));
            if (i > 1) {
                Instances[i]->SpriteTemplate->LODLevels[0]->Modules.Add(FObjectFactory::ConstructObject<UParticleModuleSize>(nullptr));
                if (i > 2) {
                    Instances[i]->SpriteTemplate->LODLevels[0]->Modules.Add(FObjectFactory::ConstructObject<UParticleModuleSpawn>(nullptr));
                    if (i > 3)
                    {
                        Instances[i]->SpriteTemplate->LODLevels[0]->Modules.Add(FObjectFactory::ConstructObject<UParticleModuleVelocity>(nullptr));
                    }
                }

            }
        }


        first = false;
    }

    // Module을 정렬
    struct EmitterModulesSorted {
        UParticleEmitter* Emitter;
        TArray<UParticleModule*> ModulesSorted;
    };

    TArray<EmitterModulesSorted> EmitterModulesSortedArray;

    static const int RequiredIndex = 0;
    static const int SpawnIndex = 1;

    // COLUMNS
    for (const FParticleEmitterInstance* Instance : Instances)
    {
        // 1. Module 정렬
        TArray<UParticleModule*> Modules = Instance->SpriteTemplate->LODLevels[0]->Modules;

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

        EmitterModulesSortedArray.Add({ Instance->SpriteTemplate, Modules });
    }

    // 3. 테이블 시작
    if (ImGui::BeginTable("EmitterModules", Instances.Num(), ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
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

        //for (const EmitterModulesSorted& EmitterModules : EmitterModulesSortedArray)
        for (int row = 0; row < MaxNumModules; ++row)
        {
            ImGui::TableNextRow();
            for (int col = 0; col < EmitterModulesSortedArray.Num(); ++col)
            {
                ImGui::TableSetColumnIndex(col);
                if (row < EmitterModulesSortedArray[col].ModulesSorted.Num())
                {
                    const UParticleModule* Module = EmitterModulesSortedArray[col].ModulesSorted[row];
                    RenderModuleCell(Module);
                }
                else
                {
                    RenderModuleCell(nullptr);
                }
            }
        }
        ImGui::EndTable();
    }
    ImGui::End();
}

void ParticlesEmitterPanel::RenderModuleCell(const UParticleModule* Module) const
{
    if (!Module) return;
    
    bool Checked = false;
    if (Module == UI->GetSelectedModule())
    {
        Checked = true;
    }
    FString ModuleName = Module->GetClass()->GetName();
    ModuleName.RemoveFromStart(TEXT("UParticleModule"));
    if (ImGui::Checkbox(*FString::Printf("##%d", Module->GetUUID()), &Checked))
    {

    }
    ImGui::SameLine();
    if (ImGui::Button(*FString::Printf("%s##%d", *ModuleName, Module->GetUUID())))
    {
        UI->SelectModule(Module);
    }

    //if (const UParticleModuleColor* ColorModule = Cast<UParticleModuleColor>(Module)) {
    //    bool Checked = false;
    //    if (ImGui::Checkbox("##ColorCheck", &Checked))
    //    {

    //    }
    //    if (ImGui::Button("Color"))
    //    {

    //    }
    //}
    //else if (const UParticleModuleLifetime* LifetimeModule = Cast<UParticleModuleLifetime>(Module)) {

    //}
    //else if (const UParticleModuleLocation* LocationModule = Cast<UParticleModuleLocation>(Module)) {
    //    ImGui::TextUnformatted("Location");
    //}
    //else if (const UParticleModuleSize* SizeModule = Cast<UParticleModuleSize>(Module)) {
    //    ImGui::TextUnformatted("Size");
    //}
    //else if (const UParticleModuleSpawn* SpawnModule = Cast<UParticleModuleSpawn>(Module)) {
    //    ImGui::TextUnformatted("Spawn");
    //}
    //else if (const UParticleModuleVelocity* VelocityModule = Cast<UParticleModuleVelocity>(Module)) {
    //    ImGui::TextUnformatted("Velocity");
    //}
    //else if (const UParticleModuleRequired* RequiredModule = Cast<UParticleModuleRequired>(Module)) {
    //    ImGui::TextUnformatted("Required");
    //}
    //else if (Module == nullptr)
    //{
    //    ImGui::TextUnformatted("-");
    //}
    //else {
    //    // 알 수 없는 타입
    //    ImGui::Text("Unknown");
    //}
}


void ParticlesEmitterPanel::OnResize(HWND hWnd)
{
    RECT ClientRect;
    GetClientRect(hWnd, &ClientRect);
    Width = static_cast<float>(ClientRect.right - ClientRect.left);
    Height = static_cast<float>(ClientRect.bottom - ClientRect.top);
}