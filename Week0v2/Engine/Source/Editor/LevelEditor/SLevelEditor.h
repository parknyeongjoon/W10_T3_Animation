#pragma once
#include "Math/Vector.h"
#include "Runtime/Engine/Classes/Engine/FEditorStateManager.h"

class SSplitterH;
class SSplitterV;
class FEditorViewportClient;

class SLevelEditor
{
public:
    SLevelEditor();
    ~SLevelEditor();
    void Initialize(HWND OwnerWindow);
    void Tick(ELevelTick tickType, double DeltaTime);
    void Release();
    
    void SelectViewport(FVector2D Point);
    void OnResize();
    void ResizeViewports();
    void SetEnableMultiViewport(bool bIsEnable);
    bool IsMultiViewport();
private:
    bool bInitialize;
    SSplitterH* HSplitter;
    SSplitterV* VSplitter;
    UWorld* World;
    TArray<std::shared_ptr<FEditorViewportClient>> ViewportClients;
    std::shared_ptr<FEditorViewportClient> ActiveViewportClient;

    /** 우클릭 시 캡처된 마우스 커서의 초기 위치 (스크린 좌표계) */
    FVector2D MousePinPosition;

    
    bool bLButtonDown = false;
    bool bRButtonDown = false;
    
    bool bMultiViewportMode;

    float EditorWidth;
    float EditorHeight;

    FEditorStateManager EditorStateManager;
public:
    TArray<std::shared_ptr<FEditorViewportClient>> GetViewports() { return ViewportClients; }
    std::shared_ptr<FEditorViewportClient> GetViewport(uint32 Index) { return ViewportClients[Index]; }
    std::shared_ptr<FEditorViewportClient> GetActiveViewportClient() const
    {
        return ActiveViewportClient;
    }
    void SetViewportClient(std::shared_ptr<FEditorViewportClient> viewportClient)
    {
        ActiveViewportClient = viewportClient;
    }
    void SetViewportClient(int index)
    {
        ActiveViewportClient = ViewportClients[index];
    }

    FEditorStateManager& GetEditorStateManager() { return EditorStateManager; }
    //Save And Load
private:
    const FString IniFilePath = "editor.ini";
public:
    void LoadConfig();
    void SaveConfig();
private:
    TMap<FString, FString> ReadIniFile(const FString& filePath);
    void WriteIniFile(const FString& filePath, const TMap<FString, FString>& config);

    template <typename T>
    T GetValueFromConfig(const TMap<FString, FString>& config, const FString& key, T defaultValue) {
        if (const FString* Value = config.Find(key))
        {
            std::istringstream iss(**Value);
            T value;
            if (iss >> value)
            {
                return value;
            }
        }
        return defaultValue;
    }
};

