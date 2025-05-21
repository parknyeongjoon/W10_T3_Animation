#pragma once
#include "Engine/Engine.h"
#include "Coroutine/CoroutineManager.h"
#include "CoreUObject/UObject/ObjectTypes.h"

class FSkeletalPreviewUI;
class FParticlePreviewUI;
class FCollisionManager;
class FRenderer;
class UEditorPlayer;
class FContentsUI;
class UnrealEd;
class SLevelEditor;

extern UWorld* GWorld;

class UEditorEngine : public UEngine
{
    DECLARE_CLASS(UEditorEngine, UEngine)
    
public:
    UEditorEngine() = default;

    void Init() override;
    void Tick(float DeltaTime) override;
    void Release() override;

    void Input();
    
    void PreparePIE();
    void StartPIE() const;
    void PausedPIE() const;
    void ResumingPIE() const;
    void StopPIE();

    void UpdateGizmos(UWorld* World) const;
    UEditorPlayer* GetEditorPlayer() const { return EditorPlayer; }
    UWorld* CreateWorld(EWorldType::Type WorldType, ELevelTick LevelTick);
    void RemoveWorld(UWorld* World);

    UWorld* CreatePreviewWindow(EViewportClientType Type, const FString& Name = FString());

public:
    static FCollisionManager CollisionManager;
    static FCoroutineManager CoroutineManager;
    bool bUButtonDown = false;

    void ForceEditorUIOnOff() { bForceEditorUI = !bForceEditorUI; }
    
    bool bForceEditorUI = false;
public:
    SLevelEditor* GetLevelEditor() const { return LevelEditor; }
    UnrealEd* GetUnrealEditor() const { return UnrealEditor; } // 메인 에디터
    FSkeletalPreviewUI* GetSkeletalPreviewUI() const { return SkeletalPreviewUI; } // 스켈레탈 + 애니메이션 에디터
    FParticlePreviewUI* GetParticlePreviewUI() const { return UI; } // 파티클 에디터

    float testBlurStrength;

private:
    std::shared_ptr<FWorldContext> CreateNewWorldContext(UWorld* InWorld, EWorldType::Type InWorldType, ELevelTick LevelType);

    // TODO 임시 Public 바꿔잇
public:
    FContentsUI* ContentsUI = nullptr; // 파이/게임 전용

    std::shared_ptr<FWorldContext> EditorWorldContext = nullptr;
    std::shared_ptr<FWorldContext> PIEWorldContext = nullptr;
    // UUID 타입 대신 WorldContexts 맵과 동일한 방식으로 정의
    TMap<int, std::shared_ptr<FWorldContext>> PreviewWorldContexts;
private:
    UnrealEd* UnrealEditor = nullptr;
    FSkeletalPreviewUI* SkeletalPreviewUI = nullptr;
    FParticlePreviewUI* UI = nullptr;

    SLevelEditor* LevelEditor = nullptr;
    UEditorPlayer* EditorPlayer = nullptr;
};
