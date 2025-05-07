#pragma once
#include "UnrealEngine.h"
#include "Engine/ResourceManager.h"
#include "LevelEditor/SlateAppMessageHandler.h"

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
    
    HWND CreateEngineWindow(HINSTANCE hInstance, WCHAR WindowClass[], WCHAR Title[]);
    void DestroyEngineWindow(HWND AppWnd, HINSTANCE hInstance, WCHAR WindowClass[]);
private:
    void UpdateUI(HWND AppWnd) const;
    
    static LRESULT CALLBACK AppWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);


public:
    FSlateAppMessageHandler* GetAppMessageHandler() const { return AppMessageHandler.get(); }
    HWND GetDefaultWindow() const { return DefaultWindow; }
    
public:
    static FGraphicsDevice GraphicDevice;
    static FRenderer Renderer;
    static FResourceManager ResourceManager;
    

private:
    TArray<HWND> AppWindows;
    HWND DefaultWindow;
    std::unique_ptr<FSlateAppMessageHandler> AppMessageHandler;
    
    bool bIsExit = false;
    const uint32 TargetFPS = 1000;
};

extern FEngineLoop GEngineLoop;
