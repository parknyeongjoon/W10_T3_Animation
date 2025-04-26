#pragma once
#include "Core/HAL/PlatformType.h"
#include "D3D11RHI/GraphicDevice.h"
#include "Engine/Engine.h"
#include "Renderer/Renderer.h"
#include "Engine/ResourceManager.h"
#include "Physics/FCollisionManager.h"

class FSceneMgr;
class UnrealEd;
class UImGuiManager;
class UWorld;
class FEditorViewportClient;
class SSplitterV;
class SSplitterH;
class SLevelEditor;

extern UWorld* GWorld;

class UEditorEngine : public UEngine
{
    DECLARE_CLASS(UEditorEngine, UEngine)
    
public:
    UEditorEngine();
    int32 Init(HWND hwnd);
    void Render();
    void Tick(float deltaSeconds);
    void Exit();
    float GetAspectRatio(IDXGISwapChain* swapChain) const;
    void Input();

    void PreparePIE();
    void StartPIE();
    void PausedPIE();
    void ResumingPIE();
    void StopPIE();
    void GetLevel();

private:
    void WindowInit(HINSTANCE hInstance);
    void ResizeGizmo();
public:
    static FGraphicsDevice graphicDevice;
    static FRenderer renderer;
    static FResourceManager ResourceManager;
    static FCollisionManager CollisionManager;
    
    HWND hWnd;
    ELevelTick levelType = ELevelTick::LEVELTICK_ViewportsOnly;
private:
    UImGuiManager* UIMgr;
    UWorld* GWorld;
    SLevelEditor* LevelEditor;
    UnrealEd* UnrealEditor;
    FSceneMgr* SceneMgr;
    
    bool bTestInput = false;
public:
    UWorld* GetWorld() const { return GWorld; }
    SLevelEditor* GetLevelEditor() const { return LevelEditor; }
    UnrealEd* GetUnrealEditor() const { return UnrealEditor; }
    FSceneMgr* GetSceneManager() const { return SceneMgr; }
};
extern UEditorEngine* GEngine;
