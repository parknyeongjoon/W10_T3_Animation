#include "Core/HAL/PlatformType.h"
#include "EditorEngine.h"
#include "LaunchEngineLoop.h"


FEngineLoop GEngineLoop; 

void RedirectIOToConsole() {
    AllocConsole();
    FILE* fp; freopen_s(&fp, "CONOUT$", "w", stdout); freopen_s(&fp, "CONOUT$", "w", stderr); freopen_s(&fp, "CONIN$", "r", stdin); std::cout << "Debug Console Initialized!" << std::endl;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{

#ifdef _DEBUG 
    RedirectIOToConsole();
#endif

    // 사용 안하는 파라미터들
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nShowCmd);

    GEngineLoop.Init(hInstance);
    GEngineLoop.Tick();
    GEngineLoop.Exit();

    return 0;

#ifdef _DEBUG
    FreeConsole();
#endif
}

