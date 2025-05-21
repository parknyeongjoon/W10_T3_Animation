#pragma once
#include "Delegates/Delegate.h"
#include "Math/Vector.h"
#include "Runtime/Engine/Classes/Engine/FEditorStateManager.h"
#include "UObject/ObjectTypes.h"

class FViewportClient;
class SSplitterH;
class SSplitterV;
class FEditorViewportClient;

struct FWindowViewportClientData
{
    float EditorWidth;
    float EditorHeight;

    bool bMultiViewportMode : 1;
    EViewportClientType ViewportClientType;
    uint32 ActiveViewportIndex;

    EControlMode ViewportControlMode;
    ECoordiMode ViewportCoordiMode;

    TArray<std::shared_ptr<FEditorViewportClient>> ViewportClients;
    
    std::shared_ptr<SSplitterH> HSplitter;
    std::shared_ptr<SSplitterV> VSplitter;

    void SetMode(const EControlMode InViewportControlMode) { ViewportControlMode = InViewportControlMode; }
    EControlMode GetControlMode() const { return ViewportControlMode; }
    ECoordiMode GetCoordiMode() const { return ViewportCoordiMode; }
    void AddControlMode() { ViewportControlMode = static_cast<EControlMode>((ViewportControlMode + 1) % CM_END); }
    void AddCoordiMode() { ViewportCoordiMode = static_cast<ECoordiMode>((ViewportCoordiMode + 1) % CDM_END); }
    
    std::shared_ptr<FEditorViewportClient> GetActiveViewportClient() { return ViewportClients[ActiveViewportIndex]; }

    FWindowViewportClientData()
        : EditorWidth(0.0f)
        , EditorHeight(0.0f)
        , bMultiViewportMode(false)
        , ViewportClientType(EViewportClientType::Editor)
        , ActiveViewportIndex(0)
        , ViewportControlMode()
        , ViewportCoordiMode()
    {
    }
};

class SLevelEditor
{
public:
    SLevelEditor() = default;
    ~SLevelEditor() = default;

    SLevelEditor(const SLevelEditor&) = delete;
    SLevelEditor& operator=(const SLevelEditor&) = delete;
    SLevelEditor(SLevelEditor&&) = delete;
    SLevelEditor& operator=(SLevelEditor&&) = delete;

    void Initialize(UWorld* World, HWND OwnerWindow);
    void Tick(double DeltaTime);
    void Release();
    
    template <typename T>
        requires std::derived_from<T, FViewportClient>
    std::shared_ptr<T> AddViewportClient(HWND OwnerWindow, UWorld* World, const EViewportClientType Type);

    int GetNumViewportClientByType(const EViewportClientType Type);

    void RemoveViewportClient(HWND OwnerWindow, const std::shared_ptr<FEditorViewportClient>& ViewportClient);
    void RemoveViewportClients(HWND OwnerWindow);

    void ResizeWindow(HWND AppWnd, FVector2D ClientSize);
    void ResizeViewports(HWND AppWnd);

    void SelectViewport(HWND AppWnd, FVector2D Point);
    void SetEnableMultiViewport(HWND AppWnd, bool bIsEnable);
    bool IsMultiViewport(HWND AppWnd);

    void RegisterEditorInputDelegates();
    void RegisterPIEInputDelegates();

    uint32 GetCurrentViewportClientIndex() const { return ActiveViewportClientIndex; }
    HWND GetCurrentViewportWindow() const { return ActiveViewportWindow; }

    TArray<std::shared_ptr<FEditorViewportClient>> GetViewportClients(const HWND AppWnd) const
    {
        if (!WindowViewportDataMap.Contains(ActiveViewportWindow))
        {
            return TArray<std::shared_ptr<FEditorViewportClient>>();
        }
        
        return WindowViewportDataMap[AppWnd].ViewportClients;
    }
    
    std::shared_ptr<FEditorViewportClient> GetActiveViewportClient() const
    {
        if (!WindowViewportDataMap.Contains(ActiveViewportWindow))
        {
            return nullptr;
        }
        return WindowViewportDataMap[ActiveViewportWindow].ViewportClients[ActiveViewportClientIndex];
    }

    FWindowViewportClientData& GetActiveViewportClientData()
    {
        if (!WindowViewportDataMap.Contains(ActiveViewportWindow))
        {
            auto Temp = FWindowViewportClientData();
            return Temp;
        }

        return WindowViewportDataMap[ActiveViewportWindow];
    }

    FWindowViewportClientData& GetViewportClientData(const HWND AppWnd)
    {
        if (!WindowViewportDataMap.Contains(AppWnd))
        {
            auto Temp = FWindowViewportClientData();
            return Temp;
        }
        return WindowViewportDataMap[AppWnd];
    }
    
    void FocusViewportClient(const HWND AppWnd, const uint32 ViewportClientIndex)
    {
        ActiveViewportWindow = AppWnd;
        ActiveViewportClientIndex = ViewportClientIndex;
    }

    FEditorStateManager& GetEditorStateManager() { return EditorStateManager; }

/* Save And Load*/
public:
    void LoadConfig();
    void SaveConfig();

private:
    TMap<FString, FString> ReadIniFile(const FString& FilePath) const;
    void WriteIniFile(const FString& FilePath, const TMap<FString, FString>& Config) const;

    template <typename T>
    T GetValueFromConfig(const TMap<FString, FString>& Config, const FString& Key, T DefaultValue) {
        if (const FString* Value = Config.Find(Key))
        {
            std::istringstream iss(**Value);
            T NewValue;
            if (iss >> NewValue)
            {
                return NewValue;
            }
        }
        return DefaultValue;
    }
/* End of Save And Load */

private:
    uint32 ActiveViewportClientIndex = 0;
    HWND ActiveViewportWindow = nullptr;
    FEditorStateManager EditorStateManager;

    TArray<FDelegateHandle> InputDelegatesHandles;

    TMap<HWND, FWindowViewportClientData> WindowViewportDataMap;

    /** 우클릭 시 캡처된 마우스 커서의 초기 위치 (스크린 좌표계) */
    FVector2D MousePinPosition;

    const FString IniFilePath = "editor.ini";
};

