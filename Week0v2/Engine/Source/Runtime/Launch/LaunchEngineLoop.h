#pragma once
#include "UnrealEngine.h"
#include "Engine/ResourceManager.h"

class ImGuiManager;
class FRenderer;

class FEngineLoop : IEngineLoop
{
public:
    FEngineLoop();
    virtual ~FEngineLoop() = default;

public:
    int32 Init(HINSTANCE hInstance) override;
    void Tick() override;
    void Render();
    void ClearPendingCleanupObjects() override;

    void Exit();
    
private:
    void WindowInit(HINSTANCE hInstance);
    void UpdateUI() const;
    
    static LRESULT CALLBACK AppWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

public:
    static FGraphicsDevice GraphicDevice; // 함
    static FRenderer Renderer; // 함
    static FResourceManager ResourceManager; // 함

    HWND AppWnd;

private:
    ImGuiManager* ImGuiUIManager; // 함
    
private:
    bool bIsExit = false;
    const uint32 TargetFPS = 1000;
};

extern FEngineLoop GEngineLoop;
