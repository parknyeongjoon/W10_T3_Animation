#include "ParticlePreviewUI.h"
#include "EditorPanel.h"

#include "PropertyEditor/Particles/ParticlesMenuBar.h"
#include "PropertyEditor/Particles/ParticlesDetailsPanel.h"
#include "PropertyEditor/Particles/ParticlesEmitterPanel.h"
#include "PropertyEditor/Particles/ParticlesCurveEditorPanel.h"

#include "Classes/Particles/ParticleEmitter.h"
#include "Classes/Particles/ParticleModuleColor.h"
#include "Classes/Particles/ParticleModuleLifetime.h"
#include "Classes/Particles/ParticleModuleLocation.h"
#include "Classes/Particles/ParticleModuleRequired.h"
#include "Classes/Particles/ParticleModuleSize.h"
#include "Classes/Particles/ParticleModuleSpawn.h"
#include "Classes/Particles/ParticleModuleVelocity.h"
#include "Classes/Particles/ParticleLODLevel.h"

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

    // TEST CASES
    for (int j = 0; j < 5; j++)
    {
        UParticleSystem* NewParticleSystem = FObjectFactory::ConstructObject<UParticleSystem>(nullptr);
        for(int i = 0; i < 5; i++)
        {
            UParticleEmitter* NewEmitter = FObjectFactory::ConstructObject<UParticleEmitter>(nullptr);
            UParticleLODLevel* NewLODLevel = FObjectFactory::ConstructObject<UParticleLODLevel>(nullptr);
            NewLODLevel->RequiredModule = FObjectFactory::ConstructObject<UParticleModuleRequired>(nullptr);
            NewLODLevel->Modules.Add(NewLODLevel->RequiredModule);
            NewLODLevel->Modules.Add(FObjectFactory::ConstructObject<UParticleModuleColor>(nullptr));
            NewLODLevel->Modules.Add(FObjectFactory::ConstructObject<UParticleModuleLifetime>(nullptr));
            NewLODLevel->Modules.Add(FObjectFactory::ConstructObject<UParticleModuleLocation>(nullptr));
            if (i > 1) {
                NewLODLevel->Modules.Add(FObjectFactory::ConstructObject<UParticleModuleSize>(nullptr));
                if (i > 2) {
                    NewLODLevel->Modules.Add(FObjectFactory::ConstructObject<UParticleModuleSpawn>(nullptr));
                    if (i > 3)
                    {
                        NewLODLevel->Modules.Add(FObjectFactory::ConstructObject<UParticleModuleVelocity>(nullptr));
                    }
                }

            }
            NewEmitter->LODLevels.Add(NewLODLevel);
            NewParticleSystem->Emitters.Add(NewEmitter);
        }
        ParticleSystems.Add(NewParticleSystem);
    }
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
    PreviewWorld = InWorld;
    for (auto& [_, Panel] : Panels)
    {
        Panel->SetWorld(PreviewWorld);
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

void FParticlePreviewUI::SetSelectedSystemIndex(int32 Index)
{
    if (Index >= 0 && Index < ParticleSystems.Num())
    {
        SelectedSystemIndex = Index;
    }
}

void FParticlePreviewUI::SetSelectedEmitterIndex(int32 Index)
{
    if (Index >= 0 && Index < ParticleSystems.Num())
    {
        SelectedEmitterIndex = Index;
    }
}

void FParticlePreviewUI::SetSelectedLODIndex(int32 Index)
{
    if (Index >= 0 && Index < ParticleSystems.Num())
    {
        SelectedLODIndex = Index;
    }
}

void FParticlePreviewUI::SetSelectedModuleIndex(int32 Index)
{
    if (Index >= 0 && Index < ParticleSystems.Num())
    {
        SelectedModuleIndex = Index;
    }
}

const UParticleSystem* FParticlePreviewUI::GetSelectedSystem() const
{
    if (SelectedSystemIndex >= 0 && SelectedSystemIndex < ParticleSystems.Num())
    {
        return ParticleSystems[SelectedSystemIndex];
    }
    return nullptr;
}

const UParticleEmitter* FParticlePreviewUI::GetSelectedEmitter() const
{
    if (SelectedSystemIndex >= 0 && SelectedSystemIndex < ParticleSystems.Num())
    {
        const UParticleSystem* ParticleSystem = ParticleSystems[SelectedSystemIndex];
        if (SelectedEmitterIndex >= 0 && SelectedEmitterIndex < ParticleSystem->Emitters.Num())
        {
            return ParticleSystem->Emitters[SelectedEmitterIndex];
        }
    }
    return nullptr;
}

const UParticleLODLevel* FParticlePreviewUI::GetSelectedLODLevel() const
{
    if (SelectedSystemIndex >= 0 && SelectedSystemIndex < ParticleSystems.Num())
    {
        const UParticleSystem* ParticleSystem = ParticleSystems[SelectedSystemIndex];
        if (SelectedEmitterIndex >= 0 && SelectedEmitterIndex < ParticleSystem->Emitters.Num())
        {
            UParticleEmitter* Emitter = ParticleSystem->Emitters[SelectedEmitterIndex];
            if (SelectedLODIndex >= 0 && SelectedLODIndex < Emitter->LODLevels.Num())
            {
                return Emitter->LODLevels[SelectedLODIndex];
            }
        }
    }
    return nullptr;
}

const UParticleModule* FParticlePreviewUI::GetSelectedModule() const
{
    if (SelectedSystemIndex >= 0 && SelectedSystemIndex < ParticleSystems.Num())
    {
        const UParticleSystem* ParticleSystem = ParticleSystems[SelectedSystemIndex];
        if (SelectedEmitterIndex >= 0 && SelectedEmitterIndex < ParticleSystem->Emitters.Num())
        {
            UParticleEmitter* Emitter = ParticleSystem->Emitters[SelectedEmitterIndex];
            if (SelectedLODIndex >= 0 && SelectedLODIndex < Emitter->LODLevels.Num())
            {
                UParticleLODLevel* LODLevel = Emitter->LODLevels[SelectedLODIndex];
                if (SelectedModuleIndex >= 0 && SelectedModuleIndex < LODLevel->Modules.Num())
                {
                    return LODLevel->Modules[SelectedModuleIndex];
                }
            }
        }
    }
    return nullptr;
}

void FParticlePreviewUI::SetSelectedSystem(const UParticleSystem* ParticleSystem)
{
    if (ParticleSystem)
    {
        if (ParticleSystems.Contains(ParticleSystem))
        {
            for (int32 i = 0; i < ParticleSystems.Num(); ++i)
            {
                if (ParticleSystems[i] == ParticleSystem)
                {
                    SelectedSystemIndex = i;
                    break;
                }
            }
        }
    }
}

void FParticlePreviewUI::SetSelectedEmitter(const UParticleEmitter* Emitter)
{
    if (Emitter)
    {
        const UParticleSystem* ParticleSystem = GetSelectedSystem();
        if (ParticleSystem)
        {
            for (int32 i = 0; i < ParticleSystem->Emitters.Num(); ++i)
            {
                if (ParticleSystem->Emitters[i] == Emitter)
                {
                    SelectedEmitterIndex = i;
                    break;
                }
            }
        }
    }
}

void FParticlePreviewUI::SetSelectedLODLevel(const UParticleLODLevel* LODLevel)
{
    if (LODLevel)
    {
        const UParticleEmitter* Emitter = GetSelectedEmitter();
        if (Emitter)
        {
            for (int32 i = 0; i < Emitter->LODLevels.Num(); ++i)
            {
                if (Emitter->LODLevels[i] == LODLevel)
                {
                    SelectedLODIndex = i;
                    break;
                }
            }
        }
    }
}

void FParticlePreviewUI::SetSelectedModule(const UParticleModule* Module)
{
    if (Module)
    {
        const UParticleLODLevel* LODLevel = GetSelectedLODLevel();
        if (LODLevel)
        {
            SelectedModuleIndex = LODLevel->Modules.Num() - 1;
        }
    }
}
