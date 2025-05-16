#include "Console.h"

#include "ImGUI/imgui.h"

#include "LaunchEngineLoop.h"
#include "Renderer/Renderer.h"


// 싱글톤 인스턴스 반환
Console& Console::GetInstance() {
    static Console instance;
    return instance;
}

void StatOverlay::DrawTextOverlay(const std::string& text, int x, int y) {
    ImGui::SetNextWindowPos(ImVec2(x, y));
    ImGui::Text("%s", text.c_str());
}

// 생성자
Console::Console() {}

// 소멸자
Console::~Console() {}

// 로그 초기화
void Console::Clear() {
    items.Empty();
}

// 로그 추가
void Console::AddLog(LogLevel level, const char* fmt, ...) {
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    items.Add({ level, std::string(buf) });
    scrollToBottom = true;
}


// 콘솔 창 렌더링
void Console::Draw() {
    if (!bWasOpen) return;
    // 창 크기 및 위치 계산
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    
    // 콘솔 창의 크기와 위치 설정
    float expandedHeight = displaySize.y * 0.4f; // 확장된 상태일 때 높이 (예: 화면의 40%)
    float collapsedHeight = 30.0f;               // 축소된 상태일 때 높이
    float currentHeight = bExpand ? expandedHeight : collapsedHeight;
    
    // 왼쪽 하단에 위치하도록 계산 (창의 좌측 하단이 화면의 좌측 하단에 위치)
    ImVec2 windowSize(displaySize.x * 0.5f, currentHeight); // 폭은 화면의 40%
    ImVec2 windowPos(0, displaySize.y - currentHeight);
    
    // 창 위치와 크기를 고정
    //ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
    //ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);
    // 창을 표시하고 닫힘 여부 확인
    overlay.Render(GEngineLoop.GraphicDevice.DeviceContext, width, height);
    bExpand = ImGui::Begin("Console", &bWasOpen);
    if (!bExpand)
    {
        ImGui::End();
        return;
    }

    // 창을 접었을 경우 UI를 표시하지 않음
    if (!bExpand)
    {
        ImGui::End();
        return;
    }
    
    // 버튼 UI (로그 수준별 추가)
    if (ImGui::Button("Clear")) { Clear(); }
    ImGui::SameLine();
    if (ImGui::Button("Copy")) { ImGui::LogToClipboard(); }

    ImGui::Separator();

    // 필터 입력 창
    ImGui::Text("Filter:");
    ImGui::SameLine();

    static ImGuiTextFilter filter;  // 필터링을 위한 ImGuiTextFilter
    
    filter.Draw("##Filter", 100);
    
    ImGui::SameLine();

    // 로그 수준을 선택할 체크박스
    ImGui::Checkbox("Show Display", &showLogTemp);
    ImGui::SameLine();
    ImGui::Checkbox("Show Warning", &showWarning);
    ImGui::SameLine();
    ImGui::Checkbox("Show Error", &showError);

    ImGui::Separator();
    // 로그 출력 (필터 적용)
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetTextLineHeightWithSpacing()), false, ImGuiWindowFlags_HorizontalScrollbar);
    for (const auto& entry : items) {
        if (!filter.PassFilter(*entry.message)) continue;

        // 로그 수준에 맞는 필터링
        if ((entry.level == LogLevel::Display && !showLogTemp) ||
            (entry.level == LogLevel::Warning && !showWarning) ||
            (entry.level == LogLevel::Error && !showError)) {
            continue;
        }

        // 색상 지정
        ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        switch (entry.level) {
        case LogLevel::Display: color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); break;  // 기본 흰색
        case LogLevel::Warning: color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f); break; // 노란색
        case LogLevel::Error:   color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); break; // 빨간색
        }

        ImGui::TextColored(color, "%s", *entry.message);
    }
    if (scrollToBottom) {
        ImGui::SetScrollHereY(1.0f);
        scrollToBottom = false;
    }
    ImGui::EndChild();

    ImGui::Separator();

    // 입력창
    bool reclaimFocus = false;
    if (ImGui::InputText("Input", inputBuf, IM_ARRAYSIZE(inputBuf), ImGuiInputTextFlags_EnterReturnsTrue)) {
        if (inputBuf[0]) {
            AddLog(LogLevel::Display, ">> %s", inputBuf);
            std::string command(inputBuf);
            ExecuteCommand(command);
            history.Add(std::string(inputBuf));
            historyPos = -1;
            scrollToBottom = true; // 자동 스크롤
        }
        inputBuf[0] = '\0';
        reclaimFocus = true;
    }

    // 입력 필드에 자동 포커스
    if (reclaimFocus) {
        ImGui::SetKeyboardFocusHere(-1);
    }

    ImGui::End();
}

void Console::ExecuteCommand(const std::string& command)
{
    AddLog(LogLevel::Display, "Executing command: %s", command.c_str());

    if (command == "clear")
    {
        Clear();
    }
    else if (command == "help")
    {
        AddLog(LogLevel::Display, "Available commands:");
        AddLog(LogLevel::Display, " - clear: Clears the console");
        AddLog(LogLevel::Display, " - help: Shows available commands");
        AddLog(LogLevel::Display, " - stat fps: Toggle FPS display");
        AddLog(LogLevel::Display, " - stat memory: Toggle Memory display");
        AddLog(LogLevel::Display, " - stat none: Hide all stat overlays");
    }
    else if (command.rfind("stat ", 0) == 0) { // stat 명령어 처리
        overlay.ToggleStat(command);
    }
    else if (command.rfind("shadow_filter ", 0) == 0) //shadow filter 명령어 처리
    {
        SetShadowFilterMode(command);
    }
    else {
        AddLog(LogLevel::Error, "Unknown command: %s", command.c_str());
    }
}

void Console::OnResize(HWND hWindow)
{
    RECT clientRect;
    GetClientRect(hWindow, &clientRect);
    width = clientRect.right - clientRect.left;
    height = clientRect.bottom - clientRect.top;
}

void Console::SetShadowFilterMode(const std::string& command)
{
    if (command == "shadow_filter VSM")
    {
        GEngineLoop.Renderer.SetShadowFilterMode(EShadowFilterMode::VSM);
    }
    else if (command == "shadow_filter PCF")
    {
        GEngineLoop.Renderer.SetShadowFilterMode(EShadowFilterMode::PCF);
    }
}

void StatOverlay::ToggleStat(const std::string& command)
{
    if (command == "stat fps") { showFPS = true; showRender = true; isOpen = true; }
    else if (command == "stat memory") { showMemory = true; showRender = true; isOpen = true; }
    else if (command == "stat shadow") { showShadow = true; showRender = true; isOpen = true; }
    else if (command == "stat none") {
        showFPS = false;
        showMemory = false;
        showRender = false;
        isOpen = false;
    }
}

void StatOverlay::Render(ID3D11DeviceContext* context, UINT width, UINT height)
{
    if (!showRender || !isOpen)
        return;

    ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    ImVec2 windowSize(displaySize.x * 0.5f, displaySize.y * 0.5f);
    ImVec2 windowPos((displaySize.x - windowSize.x) * 0.5f, (displaySize.y - windowSize.y) * 0.5f);

    //ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
    //ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));

    ImGui::Begin("Stat Overlay", &isOpen);

    if (showFPS) {
        static float lastTime = ImGui::GetTime();
        static int frameCount = 0;
        static float fps = 0.0f;

        frameCount++;
        float currentTime = ImGui::GetTime();
        float deltaTime = currentTime - lastTime;

        if (deltaTime >= 1.0f) {
            fps = frameCount / deltaTime;
            frameCount = 0;
            lastTime = currentTime;
        }
        ImGui::Text("FPS: %.2f", fps);
        ImGui::Separator();
    }

    if (showMemory) {
        ImGui::Text("Allocated Object Count: %llu", FPlatformMemory::GetAllocationCount<EAT_Object>());
        ImGui::Text("Allocated Object Memory: %llu B", FPlatformMemory::GetAllocationBytes<EAT_Object>());
        ImGui::Text("Allocated Container Count: %llu", FPlatformMemory::GetAllocationCount<EAT_Container>());
        ImGui::Text("Allocated Container Memory: %llu B", FPlatformMemory::GetAllocationBytes<EAT_Container>());

        ImGui::Separator();
    }

    if (showShadow)
    {
        FShadowMemoryUsageInfo Info = FShadowResourceFactory::GetShadowMemoryUsageInfo();
        ImGui::Text("Shadow Memory Usage Info:");
        size_t pointlightAtlasMemory = GEngineLoop.Renderer.GetAtlasMemoryUsage(ELightType::PointLight);
        size_t spotlightAtlasMemory = GEngineLoop.Renderer.GetAtlasMemoryUsage(ELightType::SpotLight);
        ImGui::Text("PointLight Atlas Memory Usage : %.2f MB", (float)pointlightAtlasMemory / (1024.f * 1024.f));
        ImGui::Text("SpotLight Atlas Memory Usage : %.2f MB", (float)spotlightAtlasMemory / (1024.f * 1024.f));

        float total = (float)(Info.TotalMemoryUsage + pointlightAtlasMemory + spotlightAtlasMemory) / (1024.f * 1024.f);
        ImGui::Text("Total Memory: %.2f MB", total);
        for (const auto& pair : Info.MemoryUsageByLightType)
        {
            switch (pair.Key)
            {
            case ELightType::DirectionalLight:
            {
                ImGui::Text("%d Directional Light", Info.LightCountByLightType[pair.Key] / 4); // cascade때문에 4개 
                float mb = (float)pair.Value / (1024.f * 1024.f);
                ImGui::Text("Memory: %.2f MB", mb);
            }
                break;
            case ELightType::PointLight:
                ImGui::Text("%d Point Light", Info.LightCountByLightType[pair.Key]);
                break;
            case ELightType::SpotLight:
                ImGui::Text("%d Spot Light", Info.LightCountByLightType[pair.Key]);
                break;
            }
        }
    }

    ImGui::PopStyleColor();
    ImGui::End();

    if (!isOpen) {
        showRender = false;
    }
}
