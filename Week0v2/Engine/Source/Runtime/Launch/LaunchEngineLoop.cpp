#include "LaunchEngineLoop.h"

#include "EditorEngine.h"
#include "Engine/Engine.h"
#include "LevelEditor/SLevelEditor.h"
#include "PropertyEditor/ViewportTypePanel.h"
#include "UnrealEd/UnrealEd.h"
#include "UObject/ObjectFactory.h"

extern UEditorEngine* GEngine;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
    {
        return true;
    }
    int zDelta = 0;
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_SIZE:
        if (GEngine== nullptr)
            break;
        if (wParam != SIZE_MINIMIZED)
        {
            //UGraphicsDevice 객체의 OnResize 함수 호출
            if (UEditorEngine::graphicDevice.SwapChain)
            {
                UEditorEngine::graphicDevice.OnResize(hWnd);
            }
            for (int i = 0; i < 4; i++)
            {
                if (GEngine->GetLevelEditor())
                {
                    if (GEngine->GetLevelEditor()->GetViewports()[i])
                    {
                        GEngine->GetLevelEditor()->GetViewports()[i]->ResizeViewport(UEditorEngine::graphicDevice.SwapchainDesc);
                    }
                }
            }
        }
     Console::GetInstance().OnResize(hWnd);
        if (GEngine->GetUnrealEditor())
        {
            GEngine->GetUnrealEditor()->OnResize(hWnd);
        }
        ViewportTypePanel::GetInstance().OnResize(hWnd);
        break;
    case WM_MOUSEWHEEL:
        if (GEngine == nullptr)
            break;
        if (ImGui::GetIO().WantCaptureMouse)
            return 0;
        zDelta = GET_WHEEL_DELTA_WPARAM(wParam); // 휠 회전 값 (+120 / -120)
        if (GEngine->GetLevelEditor())
        {
            if (GEngine->GetLevelEditor()->GetActiveViewportClient()->IsPerspective())
            {
                if (GEngine->GetLevelEditor()->GetActiveViewportClient()->GetIsOnRBMouseClick())
                {
                    GEngine->GetLevelEditor()->GetActiveViewportClient()->SetCameraSpeedScalar(
                        static_cast<float>(GEngine->GetLevelEditor()->GetActiveViewportClient()->GetCameraSpeedScalar() + zDelta * 0.01)
                    );
                }
                else
                {
                    GEngine->GetLevelEditor()->GetActiveViewportClient()->CameraMoveForward(zDelta * 0.1f);
                }
            }
            else
            {
                FEditorViewportClient::SetOthoSize(-zDelta * 0.01f);
            }
        }
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}


FEngineLoop::FEngineLoop()
{
}

FEngineLoop::~FEngineLoop()
{
}

int32 FEngineLoop::PreInit()
{
    return 0;
}

int32 FEngineLoop::Init(HINSTANCE hInstance)
{
    WindowInit(hInstance);

    if (bIsEditor)
    {
        GEngine = FObjectFactory::ConstructObject<UEditorEngine>();
    }
    else
    {
        //TODO : UENGINE으로 만들 수 있게 해주기 
        // GEngine = FObjectFactory::ConstructObject<UEngine>();
    }
    
    GEngine->Init(hWnd);
    return 0;
}

void FEngineLoop::Tick()
{
    LARGE_INTEGER frequency;
    const float targetFrameTime = 1000.0 / targetFPS; // 한 프레임의 목표 시간 (밀리초 단위)

    QueryPerformanceFrequency(&frequency);

    LARGE_INTEGER startTime, endTime;
    float deltaSeconds = 1.0f;

    while (bIsExit == false)
    {
        QueryPerformanceCounter(&startTime);

        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg); // 키보드 입력 메시지를 문자메시지로 변경
            DispatchMessage(&msg);  // 메시지를 WndProc에 전달

            if (msg.message == WM_QUIT)
            {
                bIsExit = true;
                break;
            }
        }
        GEngine->Tick(deltaSeconds);
        
        do
        {
            Sleep(0);
            QueryPerformanceCounter(&endTime);
            deltaSeconds = (endTime.QuadPart - startTime.QuadPart) * 1000.0 / frequency.QuadPart;
        }
        while (deltaSeconds < targetFrameTime);

        UEngine::GFrameCount++;
    }
}

void FEngineLoop::Exit()
{
    GEngine->Exit();
    delete GEngine;
}

void FEngineLoop::ClearPendingCleanupObjects()
{
}

void FEngineLoop::WindowInit(HINSTANCE hInstance)
{
    WCHAR WindowClass[] = L"JungleWindowClass";

    WCHAR Title[] = L"Game Tech Lab";

    WNDCLASSW wndclass = {0};
    wndclass.lpfnWndProc = WndProc;
    wndclass.hInstance = hInstance;
    wndclass.lpszClassName = WindowClass;

    RegisterClassW(&wndclass);

    hWnd = CreateWindowExW(
        0, WindowClass, Title, WS_POPUP | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1000, 1000,
        nullptr, nullptr, hInstance, nullptr
    );
}
