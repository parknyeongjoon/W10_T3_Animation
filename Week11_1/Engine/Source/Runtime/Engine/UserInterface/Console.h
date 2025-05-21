#pragma once
#include <string>

#include "D3D11RHI/GraphicDevice.h"
#include "PropertyEditor/IWindowToggleable.h"

#define UE_LOG Console::GetInstance().AddLog

enum class LogLevel
{
    Display,
    Warning,
    Error
};

class StatOverlay 
{
public:
    bool showFPS = false;
    bool showMemory = false;
    bool showRender = false;
    bool showShadow = false;

    void ToggleStat(const std::string& command);


    void Render(ID3D11DeviceContext* context, UINT width, UINT height);
    

private:
    bool isOpen = true;

    float CalculateFPS() {
        static int frameCount = 0;
        static float elapsedTime = 0.0f;
        static float lastTime = 0.0f;

        float currentTime = GetTickCount64() / 1000.0f;
        elapsedTime += (currentTime - lastTime);
        lastTime = currentTime;
        frameCount++;

        if (elapsedTime > 1.0f) {
            float fps = frameCount / elapsedTime;
            frameCount = 0;
            elapsedTime = 0.0f;
            return fps;
        }
        return 0.0f;
    }

    void DrawTextOverlay(const std::string& text, int x, int y);
};
class Console : public IWindowToggleable
{
private:
    Console();
    ~Console();
public:
    static Console& GetInstance(); // 참조 반환으로 변경

    void Clear();
    void AddLog(LogLevel level, const char* fmt, ...);
    void Draw();
    void ExecuteCommand(const std::string& command);
    void OnResize(HWND hWnd);
    void Toggle() override { 
        if (bWasOpen) {
            bWasOpen = false;
        }
        else {
            bWasOpen = true;
        }
    } // Toggle() 구현 
    void SetShadowFilterMode(const std::string& command);
public:
    struct LogEntry {
        LogLevel level;
        FString message;
    };

    TArray<LogEntry> items;
    TArray<FString> history;
    int32 historyPos = -1;
    char inputBuf[256] = "";
    bool scrollToBottom = false;
    
    // 추가된 멤버 변수들
    bool showLogTemp = true;   // LogTemp 체크박스
    bool showWarning = true;   // Warning 체크박스
    bool showError = true;     // Error 체크박스

    bool bWasOpen = true;
    bool showFPS = false;
    bool showMemory = false;
    // 복사 방지
    Console(const Console&) = delete;
    Console& operator=(const Console&) = delete;
    Console(Console&&) = delete;
    Console& operator=(Console&&) = delete;

    StatOverlay overlay;
private:
    bool bExpand = true;
    UINT width;
    UINT height;

};
