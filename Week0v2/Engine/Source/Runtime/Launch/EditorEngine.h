#pragma once
#include "Engine/Engine.h"
#include "Coroutine/CoroutineManager.h"

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
    UEditorEngine();
    void Init() override;
    void Tick(float DeltaTime) override;
    void Release() override;
    void Input();
    
    void PreparePIE();
    void StartPIE();
    void PausedPIE();
    void ResumingPIE();
    void StopPIE();

    void UpdateGizmos(UWorld* World);
    UEditorPlayer* GetEditorPlayer() const { return EditorPlayer; }
    UWorld* CreateWorld(EWorldType::Type WorldType, ELevelTick LevelTick);

public:
    static FCollisionManager CollisionManager;
    static FCoroutineManager CoroutineManager;
    bool bUButtonDown = false;

    void ForceEditorUIOnOff() { bForceEditorUI = !bForceEditorUI; }
    
    bool bForceEditorUI = false;
public:
    // UISOO TODO NOW
    SLevelEditor* GetLevelEditor() const { return LevelEditor; }
    UnrealEd* GetUnrealEditor() const { return UnrealEditor; }    

    float testBlurStrength;

private:
    std::shared_ptr<FWorldContext> CreateNewWorldContext(EWorldType::Type InWorldType, ELevelTick LevelType);

    // TODO 임시 Public 바꿔잇
public:
    UnrealEd* UnrealEditor = nullptr;
    FContentsUI* ContentsUI = nullptr;
    
    std::shared_ptr<FWorldContext> PIEWorldContext = nullptr;
    std::shared_ptr<FWorldContext> EditorWorldContext = nullptr;
private:
    
    SLevelEditor* LevelEditor = nullptr;
    UEditorPlayer* EditorPlayer = nullptr;
};