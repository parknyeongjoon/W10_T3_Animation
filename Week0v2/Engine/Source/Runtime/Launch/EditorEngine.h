#pragma once
#include "Engine/Engine.h"
#include "EngineBaseTypes.h"
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
    void Init(HWND hWnd) override;
    void Tick(float deltaSeconds) override;
    void Release() override;
    void Input();

    UWorld* GetWorld() override;
    
    void PreparePIE();
    void StartPIE();
    void PausedPIE();
    void ResumingPIE();
    void StopPIE();

    
    void UpdateGizmos();
    UEditorPlayer* GetEditorPlayer() const { return EditorPlayer; }

public:

    static FCollisionManager CollisionManager;
    static FCoroutineManager CoroutineManager;
    ELevelTick LevelType = ELevelTick::LEVELTICK_ViewportsOnly;
    bool bUButtonDown = false;

    void ForceEditorUIOnOff() { bForceEditorUI = !bForceEditorUI; }
    
    bool bForceEditorUI = false;
public:
    UWorld* GetWorld() const { return ActiveWorld; }
    SLevelEditor* GetLevelEditor() const { return LevelEditor; }
    UnrealEd* GetUnrealEditor() const { return UnrealEditor; }    

    float testBlurStrength;

private:
    std::shared_ptr<FWorldContext> CreateNewWorldContext(EWorldType::Type InWorldType);

private:
    UWorld* PIEWorld = nullptr;
    UWorld* EditorWorld = nullptr;
    
    SLevelEditor* LevelEditor;
    UnrealEd* UnrealEditor;

    UWorld* ActiveWorld;

    FContentsUI* ContentsUI;
    
    bool bIsMKeyDown = false;
    UEditorPlayer* EditorPlayer = nullptr;
};