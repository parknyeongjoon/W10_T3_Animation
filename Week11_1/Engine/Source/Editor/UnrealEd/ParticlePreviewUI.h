#pragma once
#include "Container/Map.h"
#include "Container/String.h"
#include "Engine/World.h"
#include "Engine/Particles/ParticleEmitterInstances.h"
#include "Engine/Classes/Particles/ParticleSystemWorldManager.h"
#include "Engine/Classes/Particles/ParticleSystem.h"
#include "Particles/ParticleLODLevel.h"
#include "EditorViewportClient.h"

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

    void ResizeViewport(std::shared_ptr<FEditorViewportClient> ViewportClient);

    void AddEditorPanel(const FString& PanelId, const std::shared_ptr<UEditorPanel>& EditorPanel);
    std::shared_ptr<UEditorPanel> GetEditorPanel(const FString& PanelId);

    TArray<UParticleSystem*> GetParticleSystems() const { return ParticleSystems; }
    TArray<UParticleSystem*>& GetParticleSystemsRef() { return ParticleSystems; }

    void CreateEmptyParticleSystem(UObject* InOuter = nullptr);
    void AddParticleSytstem(UParticleSystem* ParticleSystem);
    int32 RemoveParticleSystem(UParticleSystem* ParticleSystem);
    void RemoveParticleSystemAt(int32 Index);

    int GetSelectedSystemIndex() const { return SelectedSystemIndex; }
    int GetSelectedEmitterIndex() const { return SelectedEmitterIndex; }
    int GetSelectedLODIndex() const { return SelectedLODIndex; }
    int GetSelectedModuleIndex() const { return SelectedModuleIndex; }

    void SetSelectedSystemIndex(int32 Index);
    void SetSelectedEmitterIndex(int32 Index);
    void SetSelectedLODIndex(int32 Index);
    void SetSelectedModuleIndex(int32 Index);

    UParticleSystem* GetSelectedSystem() const;
    UParticleEmitter* GetSelectedEmitter() const;
    UParticleLODLevel* GetSelectedLODLevel() const;
    UParticleModule* GetSelectedModule() const;

    UWorld* GetPreviewWorld() const { return PreviewWorld; }
    void SetPreviewWorld(UWorld * InWorld) { PreviewWorld = InWorld; }

    void SetSelectedSystem(const UParticleSystem* ParticleSystem);
    void SetSelectedEmitter(const UParticleEmitter* Emitter);
    void SetSelectedLODLevel(const UParticleLODLevel* onstLODLevel);
    void SetSelectedModule(const UParticleModule* Module);

    bool IsSoloEnabledAny() const { return IsSoloAny; }
    bool IsSolo(UObject* InObject) const
    {
        return SoloFlags[InObject];
    }
    void SetSolo(UObject* InObject, bool Value)
    {
        SoloFlags[InObject] = Value;
    }
    bool IsEnabled(UObject* InObject) const
    {
        return EnableFlags[InObject];
    }
    void SetEnabled(UObject* InObject, bool Value)
    {
        EnableFlags[InObject] = Value;
    }

    void RegisterFlags(UObject* InObject);
    void RemoveFlags(UObject* InObject);

    const float PreviewScreenWidth = 0.4f;
    const float PreviewScreenHeight = 0.4f;
private:
    TMap<FString, std::shared_ptr<UEditorPanel>> Panels;
    UWorld* PreviewWorld = nullptr;

    TArray<UParticleSystem*> ParticleSystems;
    int SelectedSystemIndex = 0;
    int SelectedEmitterIndex = 0;
    int SelectedLODIndex = 0;
    int SelectedModuleIndex = 0;


    // 우선순위는 SoloFlag > EnableFlag
    // IsSolo가 true이면 SoloFlags를 기준으로, IsSolo가 false이면 EnableFlags를 기준으로 활성화 여부를 결정
    // Module, Emitter, System 모두 포인터로 처리합니다.
    TMap<UObject*, bool> EnableFlags; // false인 것은 비활성화
    TMap<UObject*, bool> SoloFlags; // true인것만 활성화
    bool IsSoloAny = false; // SoloFlag가 하나라도 선택되어있는지 여부
};
