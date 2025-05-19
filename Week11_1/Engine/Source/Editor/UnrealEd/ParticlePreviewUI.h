#pragma once
#include "Container/Map.h"
#include "Container/String.h"
#include "Engine/World.h"
#include "Engine/Paritcles/ParticleEmitterInstances.h"
#include "Engine/Classes/Particles/ParticleSystemWorldManager.h"
#include "Engine/Classes/Particles/ParticleSystem.h"
#include "Particles/ParticleLODLevel.h"

class SLevelEditor;
class UEditorPanel;

class FParticlePreviewUI
{
public:
    FParticlePreviewUI() = default;
    ~FParticlePreviewUI() = default;
    void Initialize(SLevelEditor* LevelEditor, float Width, float Height);

    void Render() const;
    void OnResize(HWND hWnd) const;
    void SetWorld(UWorld* InWorld);

    void AddEditorPanel(const FString& PanelId, const std::shared_ptr<UEditorPanel>& EditorPanel);
    std::shared_ptr<UEditorPanel> GetEditorPanel(const FString& PanelId);

    TArray<const UParticleSystem*> GetParticleSystems() const { return ParticleSystems; }
    TArray<const UParticleSystem*>& GetParticleSystemsRef() { return ParticleSystems; }

    void CreateEmptyParticleSystem(UObject* InOuter = nullptr)
    {
        UParticleSystem* NewParticleSystem = FObjectFactory::ConstructObject<UParticleSystem>(InOuter);
        ParticleSystems.Add(NewParticleSystem);
    }
    void AddParticleSytstem(UParticleSystem* ParticleSystem) { ParticleSystems.Add(ParticleSystem); }
    int32 RemoveParticleSystem(UParticleSystem* ParticleSystem) { return ParticleSystems.Remove(ParticleSystem); }
    void RemoveParticleSystemAt(int32 Index) { if (Index >= 0 && Index < ParticleSystems.Num()) { ParticleSystems.RemoveAt(Index); } }

    int GetSelectedSystemIndex() const { return SelectedSystemIndex; }
    int GetSelectedEmitterIndex() const { return SelectedEmitterIndex; }
    int GetSelectedLODIndex() const { return SelectedLODIndex; }
    int GetSelectedModuleIndex() const { return SelectedModuleIndex; }

    void SetSelectedSystemIndex(int32 Index);
    void SetSelectedEmitterIndex(int32 Index);
    void SetSelectedLODIndex(int32 Index);
    void SetSelectedModuleIndex(int32 Index);

    const UParticleSystem* GetSelectedSystem() const;
    const UParticleEmitter* GetSelectedEmitter() const;
    const UParticleLODLevel* GetSelectedLODLevel() const;
    const UParticleModule* GetSelectedModule() const;

    UWorld* GetPreviewWorld() const { return PreviewWorld; }
    void SetPreviewWorld(UWorld * InWorld) { PreviewWorld = InWorld; }

    void SetSelectedSystem(const UParticleSystem* ParticleSystem);
    void SetSelectedEmitter(const UParticleEmitter* Emitter);
    void SetSelectedLODLevel(const UParticleLODLevel* LODLevel);
    void SetSelectedModule(const UParticleModule* Module);

private:
    TMap<FString, std::shared_ptr<UEditorPanel>> Panels;
    UWorld* PreviewWorld = nullptr;

    TArray<const UParticleSystem*> ParticleSystems;
    int SelectedSystemIndex = 0;
    int SelectedEmitterIndex = 0;
    int SelectedLODIndex = 0;
    int SelectedModuleIndex = 0;
};
