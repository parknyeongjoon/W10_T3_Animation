#pragma once
#include "UnrealEngine.h"
#include "Engine/ResourceManager.h"
#include "LevelEditor/SlateAppMessageHandler.h"

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
    FSlateAppMessageHandler* GetAppMessageHandler() const { return AppMessageHandler.get(); }
    
public:
    static FGraphicsDevice GraphicDevice;
    static FRenderer Renderer;
    static FResourceManager ResourceManager;

    HWND AppWnd;

private:
    std::unique_ptr<FSlateAppMessageHandler> AppMessageHandler;
    ImGuiManager* ImGuiUIManager;
    
    bool bIsExit = false;
    const uint32 TargetFPS = 1000;
};

extern FEngineLoop GEngineLoop;
