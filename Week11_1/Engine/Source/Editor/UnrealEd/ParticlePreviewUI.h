#pragma once
#include "Container/Map.h"
#include "Container/String.h"
#include "Engine/World.h"
#include "Engine/Paritcles/ParticleEmitterInstances.h"
#include "Engine/Classes/Particles/ParticleSystemWorldManager.h"
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

    TArray<FParticleEmitterInstance*>& GetParticleEmitterInstances()
    {
        return ParticleEmitterInstances;
    }

    void CreateEmptyParticleEmitterInstance()  
    {  
       FParticleEmitterInstance* NewEmitterInstance = new FParticleEmitterInstance();  
       NewEmitterInstance->EmitterName = FName(*FString::Printf(TEXT("Emitter %d"), ParticleEmitterInstances.Num()));  
       ParticleEmitterInstances.Add(NewEmitterInstance);  
    }

    void AddParticleEmitterInstance(FParticleEmitterInstance* EmitterInstance)
    {
        ParticleEmitterInstances.Add(EmitterInstance);
    }

    void RemoveParticleEmitterInstance(int index)
    {
        if (index >= 0 && index < ParticleEmitterInstances.Num())
        {
            ParticleEmitterInstances.RemoveAt(index);
        }
    }

    int GetSelectedParticle() const
    {
        return SelectedParticle;
    }
    FParticleEmitterInstance* GetSelectedParticleInstance() const
    {
        if (SelectedParticle >= 0 && SelectedParticle < ParticleEmitterInstances.Num())
        {
            return ParticleEmitterInstances[SelectedParticle];
        }
        return nullptr;
    }
    void SetSelectedParticle(int index)
    {
        if (index >= 0 && index < ParticleEmitterInstances.Num())
        {
            SelectedParticle = index;
        }
    }
    FParticleEmitterInstance* GetParticleEmitterInstance(int index) const
    {
        if (index >= 0 && index < ParticleEmitterInstances.Num())
        {
            return ParticleEmitterInstances[index];
        }
        return nullptr;
    }
    int GetSelectedModuleIndex() const
    {
        return SelectedModuleIndex;
    }
    void SetSelectedModule(int index)
    {
        if (index >= 0 && index < ParticleEmitterInstances.Num())
        {
            SelectedModuleIndex = index;
        }
    }
    UParticleModule* GetSelectedModule()
    {
        if (SelectedParticle >= 0 && SelectedParticle < ParticleEmitterInstances.Num())
        {
            FParticleEmitterInstance* EmitterInstance = ParticleEmitterInstances[SelectedParticle];
            if (EmitterInstance && EmitterInstance->SpriteTemplate && EmitterInstance->SpriteTemplate->LODLevels.Num() > 0)
            {
                UParticleLODLevel* LODLevel = EmitterInstance->SpriteTemplate->LODLevels[0];
                if (SelectedModuleIndex >= 0 && SelectedModuleIndex < LODLevel->Modules.Num())
                {
                    return LODLevel->Modules[SelectedModuleIndex];
                }
            }
        }
        return nullptr;
    }

    void SelectModule(int ParticleIndex, int ModuleIndex)
    {
        if (ParticleIndex >= 0 && ParticleIndex < ParticleEmitterInstances.Num())
        {
            SelectedParticle = ParticleIndex;
            SelectedModuleIndex = ModuleIndex;
        }
    }
    void SelectModule(const UParticleModule* Module)
    {
        if (Module)
        {
            for (int i = 0; i < ParticleEmitterInstances.Num(); ++i)
            {
                FParticleEmitterInstance* EmitterInstance = ParticleEmitterInstances[i];
                if (EmitterInstance && EmitterInstance->SpriteTemplate && EmitterInstance->SpriteTemplate->LODLevels.Num() > 0)
                {
                    UParticleLODLevel* LODLevel = EmitterInstance->SpriteTemplate->LODLevels[0];
                    for (int j = 0; j < LODLevel->Modules.Num(); ++j)
                    {
                        if (LODLevel->Modules[j] == Module)
                        {
                            SelectedParticle = i;
                            SelectedModuleIndex = j;
                            return;
                        }
                    }
                }
            }
        }
    }

    int CurrentLODLevel = 0;

private:
    TMap<FString, std::shared_ptr<UEditorPanel>> Panels;
    UWorld* World = nullptr;

    TArray<FParticleEmitterInstance*> ParticleEmitterInstances;
    int SelectedParticle = 0; // -1일때는 아무것도 선택하지 않은 상태
    int SelectedModuleIndex = 0; // -1일때는 아무것도 선택하지 않은 상태
};
