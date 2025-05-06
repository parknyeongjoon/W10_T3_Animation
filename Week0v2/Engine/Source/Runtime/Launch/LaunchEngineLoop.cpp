#include "LaunchEngineLoop.h"

#include "ImGuiManager.h"
#include "Contents/UI/ContentsUI.h"
#include "LevelEditor/SLevelEditor.h"
#include "PropertyEditor/ViewportTypePanel.h"
#include "Renderer/Renderer.h"
#include "UnrealEd/UnrealEd.h"
#include "UObject/Casts.h"


FGraphicsDevice FEngineLoop::GraphicDevice;
FRenderer FEngineLoop::Renderer;
FResourceManager FEngineLoop::ResourceManager;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

FEngineLoop::FEngineLoop() : ImGuiUIManager(nullptr)
{
}

int32 FEngineLoop::Init(HINSTANCE hInstance)
{
    WCHAR EngineWindowClass[] = L"JungleWindowClass";
    WCHAR EngineTitle[] = L"GTL TTAL KKAK";
    WCHAR EnginePreviewWindowClass[] = L"PreviewWindowClass";
    WCHAR EnginePreviewTitle[] = L"Preview";

    
    AppMessageHandler = std::make_unique<FSlateAppMessageHandler>();
    
    CreateEngineWindow(hInstance, EngineWindowClass, EngineTitle);
    CreateEngineWindow(hInstance, EnginePreviewWindowClass, EnginePreviewTitle);

    ImGuiManager* ImGuiUIManager = new ImGuiManager();
    ImGuiUIManager->Initialize(GetDefaultWindow(), GraphicDevice.Device, GraphicDevice.DeviceContext);
    
    Renderer.Initialize(&GraphicDevice);
    ResourceManager.Initialize(&GraphicDevice);

    
    // if (bIsEditor)
    {
        GEngine = FObjectFactory::ConstructObject<UEditorEngine>(nullptr);
    }
    //else
    {
        //TODO : UENGINE으로 만들 수 있게 해주기 
        // GEngine = FObjectFactory::ConstructObject<UEngine>();
    }

    GEngine->Init();

    for (auto& AppWnd : AppWindows)
    {
        UpdateUI(AppWnd);
    }
    
    return 0;
}

void FEngineLoop::Tick()
{
    LARGE_INTEGER Frequency;
    const float targetFrameTime = 1000.0 / TargetFPS; // 한 프레임의 목표 시간 (밀리초 단위)

    QueryPerformanceFrequency(&Frequency);

    LARGE_INTEGER StartTime, EndTime;
    double ElapsedTime = 0.0;

    while (bIsExit == false)
    {
        QueryPerformanceCounter(&StartTime);

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
        const float DeltaTime = static_cast<float>(ElapsedTime / 1000.f);

        GEngine->Tick(DeltaTime);

        Render();
        
        do
        {
            Sleep(0);
            QueryPerformanceCounter(&EndTime);
            ElapsedTime = static_cast<double>(EndTime.QuadPart - StartTime.QuadPart) * 1000.0 / static_cast<double>(Frequency.QuadPart);

        }
        while (ElapsedTime < targetFrameTime);

        UEngine::GFrameCount++;
    }
}

void FEngineLoop::Render()
{
    if (UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine))
    {
        if (SLevelEditor* LevelEditor = EditorEngine->GetLevelEditor())
        {
            ImGuiUIManager->BeginFrame();
            for (auto& AppWindow : AppWindows)
            {
                if (LevelEditor->GetEditorStateManager().GetEditorState() != EEditorState::Playing || EditorEngine->bForceEditorUI == true)
                {
                    EditorEngine->UnrealEditor->Render();
                }
                else
                {
                    EditorEngine->ContentsUI->Render();
                }
                GraphicDevice.Prepare(AppWindow);
                if (LevelEditor->IsMultiViewport())
                {
                    const std::shared_ptr<FEditorViewportClient> OriginViewportClient = LevelEditor->GetActiveViewportClient();
                    for (std::shared_ptr<FEditorViewportClient>& ViewportClient : LevelEditor->GetViewports())
                    {
                        LevelEditor->SetViewportClient(ViewportClient);
                        EditorEngine->UpdateGizmos();
                        Renderer.Render(ViewportClient);
                    }
                    LevelEditor->SetViewportClient(OriginViewportClient);
                }
                else
                {
                    EditorEngine->UpdateGizmos();
                    Renderer.Render(LevelEditor->GetActiveViewportClient());
                }
                

                Console::GetInstance().Draw();        

                // Pending 처리된 오브젝트 제거
                //GUObjectArray.ProcessPendingDestroyObjects();
    
                GraphicDevice.SwapBuffer(AppWindow);
            }
            ImGuiUIManager->EndFrame();
        }
    }
}

void FEngineLoop::Exit()
{
    ImGuiUIManager->Shutdown();
    delete ImGuiUIManager;
    
    ResourceManager.Release();
    Renderer.Release();
    GraphicDevice.Release();
    
    GEngine->Release();
}

void FEngineLoop::ClearPendingCleanupObjects()
{
}

void FEngineLoop::CreateEngineWindow(HINSTANCE hInstance, WCHAR WindowClass[], WCHAR Title[])
{
    WNDCLASSW wc{};
    wc.lpfnWndProc = AppWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = WindowClass;
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;

    RegisterClassW(&wc);

    HWND AppWnd = CreateWindowExW(
        0, WindowClass, Title, WS_POPUP | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1400, 1000,
        nullptr, nullptr, hInstance, nullptr
    );
    AppWindows.Add(AppWnd);

    // 순서 조심
    GraphicDevice.AddWindow(AppWnd);
    AppMessageHandler->AddWindow(AppWnd);

    UpdateUI(AppWnd);
}

void FEngineLoop::DestroyEngineWindow(HWND AppWnd)
{
    AppWindows.Remove(AppWnd);
}

void FEngineLoop::UpdateUI(HWND AppWnd) const
{
    Console::GetInstance().OnResize(AppWnd);
    if (UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine))
    {
        if (UnrealEd* UnrealEditor = EditorEngine->GetUnrealEditor())
        {
            UnrealEditor->OnResize(AppWnd);
        }
    }
}

LRESULT CALLBACK FEngineLoop::AppWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
    {
        return true;
    }
    
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_SIZE:
        if (wParam != SIZE_MINIMIZED)
        {
            FEngineLoop::GraphicDevice.OnResize(hWnd);
            
            if (UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine))
            {
                if (SLevelEditor* LevelEditor = EditorEngine->GetLevelEditor())
                {
                    for (std::shared_ptr<FEditorViewportClient>& ViewportClient : LevelEditor->GetViewports())
                    {
                        FWindowData& WindowData = FEngineLoop::GraphicDevice.SwapChains[hWnd];
                        ViewportClient->ResizeViewport(WindowData.screenWidth, WindowData.screenHeight);
                    }
                }   
            }
        }
        ViewportTypePanel::GetInstance().OnResize(hWnd);
        GEngineLoop.UpdateUI(hWnd);
        break;
    default:
        GEngineLoop.AppMessageHandler->ProcessMessage(hWnd, message, wParam, lParam);
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}