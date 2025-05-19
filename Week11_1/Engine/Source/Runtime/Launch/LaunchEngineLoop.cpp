#include "LaunchEngineLoop.h"

#include "ImGuiManager.h"
#include "WindowsCursor.h"
#include "Actors/ADodge.h"
#include "Contents/UI/ContentsUI.h"
#include "ImGUI/imgui.h"
#include "Engine/FBXLoader.h"
#include "LevelEditor/SLevelEditor.h"
#include "PropertyEditor/ViewportTypePanel.h"
#include "Renderer/Renderer.h"
#include "UnrealEd/SkeletalPreviewUI.h"
#include "UnrealEd/ParticlePreviewUI.h"
#include "UnrealEd/UnrealEd.h"
#include "UObject/Casts.h"

FGraphicsDevice FEngineLoop::GraphicDevice;
FRenderer FEngineLoop::Renderer;
FResourceManager FEngineLoop::ResourceManager;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

int32 FEngineLoop::Init(const HINSTANCE hInstance)
{
    WCHAR EngineWindowClass[] = L"JungleWindowClass";
    WCHAR EngineTitle[] = L"GTL TTAL KKAK";
    
    AppMessageHandler = std::make_unique<FSlateAppMessageHandler>();
    
    DefaultWindow = CreateEngineWindow(hInstance, EngineWindowClass, EngineTitle);
    
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

    for (const auto& AppWnd : AppWindows)
    {
        UpdateUI(AppWnd);
    }
    
    return 0;
}

void FEngineLoop::Tick()
{
    LARGE_INTEGER Frequency;
    const float TargetFrameTime = 1000.0f / TargetFPS; // 한 프레임의 목표 시간 (밀리초 단위)

    QueryPerformanceFrequency(&Frequency);

    LARGE_INTEGER StartTime, EndTime;
    double ElapsedTime = 0.0;

    while (bIsExit == false)
    {
        QueryPerformanceCounter(&StartTime);

        MSG Msg;
        while (PeekMessage(&Msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&Msg); // 키보드 입력 메시지를 문자메시지로 변경
            DispatchMessage(&Msg);  // 메시지를 WndProc에 전달

            if (Msg.message == WM_QUIT && !AppWindows.Contains(DefaultWindow))
            {
                bIsExit = true;
                break;
            }
        }
        const float DeltaTime = static_cast<float>(ElapsedTime / 1000.f);

        GEngine->Tick(DeltaTime);

        Render();

        GUObjectArray.ProcessPendingDestroyObjects();
        
        do
        {
            Sleep(0);
            QueryPerformanceCounter(&EndTime);
            ElapsedTime = static_cast<double>(EndTime.QuadPart - StartTime.QuadPart) * 1000.0 / static_cast<double>(Frequency.QuadPart);

        }
        while (ElapsedTime < TargetFrameTime);

        UEngine::GFrameCount++;
    }
}

void FEngineLoop::Render() const
{
    const UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
    if (EditorEngine == nullptr)
    {
        return;
    }

    SLevelEditor* LevelEditor = EditorEngine->GetLevelEditor();
    if (LevelEditor == nullptr)
    {
        return;
    }

    const uint32 OriginalIndex = LevelEditor->GetCurrentViewportClientIndex();
    const HWND OriginalWindow = LevelEditor->GetCurrentViewportWindow();
    TArray<HWND> CopiedAppWindows = AppWindows;
    for (const auto& AppWindow : CopiedAppWindows)
    {
        LevelEditor->FocusViewportClient(AppWindow, 0);
        TArray<std::shared_ptr<FEditorViewportClient>> ViewportClients = LevelEditor->GetViewportClients(AppWindow);
        
        GraphicDevice.Prepare(AppWindow);
        if (LevelEditor->IsMultiViewport(AppWindow))
        {
            for (uint32 i = 0; i < ViewportClients.Num(); i++)
            {
                std::shared_ptr<FEditorViewportClient> ViewportClient = ViewportClients[i];
                LevelEditor->FocusViewportClient(AppWindow, i);
                EditorEngine->UpdateGizmos(ViewportClient->GetWorld());
                Renderer.Render(ViewportClient);
            }
        }
        // 왼쪽 위에만 띄어야해서 부득이하게 여기서 설정...
        else if (LevelEditor->GetViewportClientData(AppWindow).ViewportClientType == EditorPreviewParticle)
        {
            EditorEngine->GetParticlePreviewUI()->ResizeViewport(ViewportClients[0]);
            LevelEditor->FocusViewportClient(AppWindow, 0);
            EditorEngine->UpdateGizmos(ViewportClients[0]->GetWorld());
            Renderer.Render(ViewportClients[0]);
        }
        else
        {
            for (uint32 i = 0; (i < ViewportClients.Num() && i < 1); i++)
            {
                std::shared_ptr<FEditorViewportClient> ViewportClient = ViewportClients[i];
                LevelEditor->FocusViewportClient(AppWindow, i);
                EditorEngine->UpdateGizmos(ViewportClient->GetWorld());
                Renderer.Render(ViewportClient);
            }
        }

        ImGuiManager::Get().BeginFrame(AppWindow);
        if (ViewportClients.Num() > 0)
        {
            // TODO 다른 방법으로 World 구하기
            UWorld* TargetWorld = ViewportClients[0]->GetWorld();
            EditorEngine->GetUnrealEditor()->SetWorld(TargetWorld);
            EditorEngine->ContentsUI->SetWorld(TargetWorld);
            if (TargetWorld->WorldType == EWorldType::Editor)
            {
                if (LevelEditor->GetEditorStateManager().GetEditorState() != EEditorState::Playing || EditorEngine->bForceEditorUI == true)
                {
                    EditorEngine->GetUnrealEditor()->Render();
                    Console::GetInstance().Draw();
                }
                else
                {
                    // EditorEngine->ContentsUI->Render();
                }   
            }
            else if (TargetWorld->WorldType == EWorldType::PIE)
            {
                EditorEngine->ContentsUI->Render();
                if (LevelEditor->GetEditorStateManager().GetEditorState() != EEditorState::Playing || EditorEngine->bForceEditorUI == true)
                {
                    EditorEngine->GetUnrealEditor()->RenderInPIE();
                }
                else
                {
                }
            }
            else if (TargetWorld->WorldType == EWorldType::EditorPreview)
            {
                EViewportClientType Type = LevelEditor->GetViewportClientData(AppWindow).ViewportClientType;
                switch(Type)
                {
                case EViewportClientType::EditorPreviewSkeletal:
                    EditorEngine->GetSkeletalPreviewUI()->SetWorld(TargetWorld);
                    EditorEngine->GetSkeletalPreviewUI()->Render();
                    break;
                case EViewportClientType::EditorPreviewParticle:
                    EditorEngine->GetParticlePreviewUI()->SetWorld(TargetWorld);
                    EditorEngine->GetParticlePreviewUI()->Render();
                    break;
                default:
                    assert(false);
                }
            }
        }
        ImGuiManager::Get().EndFrame(AppWindow);
    
        // Pending 처리된 오브젝트 제거
        GUObjectArray.ProcessPendingDestroyObjects();
        GraphicDevice.SwapBuffer(AppWindow);
    }
    LevelEditor->FocusViewportClient(OriginalWindow, OriginalIndex);
}

void FEngineLoop::Exit() const
{
    ImGuiManager::Get().Release();
    
    ResourceManager.Release();
    Renderer.Release();
    GraphicDevice.Release();
    
    GEngine->Release();
}

void FEngineLoop::ClearPendingCleanupObjects()
{
}

HWND FEngineLoop::CreateEngineWindow(const HINSTANCE hInstance, WCHAR WindowClass[], WCHAR Title[])
{
    WNDCLASSW wc{};
    wc.lpfnWndProc = AppWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = WindowClass;
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;

    RegisterClassW(&wc);

    const HWND AppWnd = CreateWindowExW(
        0, WindowClass, Title, WS_POPUP | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1400, 1000,
        nullptr, nullptr, hInstance, nullptr
    );
    AppWindows.Add(AppWnd);

    // 순서 조심
    GraphicDevice.AddWindow(AppWnd);

    ImGuiManager::Get().AddWindow(AppWnd, GraphicDevice.Device, GraphicDevice.DeviceContext);

    UpdateUI(AppWnd);

    return AppWnd;
}

void FEngineLoop::DestroyEngineWindow(const HWND AppWnd, const HINSTANCE hInstance, WCHAR WindowClass[])
{
    DestroyWindow(AppWnd);
    UnregisterClassW(WindowClass, hInstance);
    AppWindows.Remove(AppWnd);
    GraphicDevice.RemoveWindow(AppWnd);
    
    ImGuiManager::Get().RemoveWindow(AppWnd);
}

void FEngineLoop::UpdateUI(HWND AppWnd) const
{
    Console::GetInstance().OnResize(AppWnd);
    if (UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine))
    {
        if (const UnrealEd* UnrealEditor = EditorEngine->GetUnrealEditor())
        {
            UnrealEditor->OnResize(AppWnd);
        }

        if (const FSkeletalPreviewUI* SkeletalPreviewUI = EditorEngine->GetSkeletalPreviewUI())
        {
            SkeletalPreviewUI->OnResize(AppWnd);
        }

        if (EditorEngine->ContentsUI)
        {
            EditorEngine->ContentsUI->OnResize(AppWnd);
        }
    }
}

LRESULT CALLBACK FEngineLoop::AppWndProc(const HWND hWnd, const UINT Msg, const WPARAM wParam, const LPARAM lParam)
{
    if (ImGuiContext* TargetContext = ImGuiManager::Get().GetImGuiContext(hWnd))
    {
        ImGuiContext* OriginalContext = ImGui::GetCurrentContext();
        ImGui::SetCurrentContext(TargetContext);
        if (ImGui_ImplWin32_WndProcHandler(hWnd, Msg, wParam, lParam))
        {
            ImGui::SetCurrentContext(OriginalContext);
            return true;
        }
        ImGui::SetCurrentContext(OriginalContext);
    }
    
    switch (Msg)
    {
        case WM_CLOSE:
        {
            const auto hInstance = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(hWnd, GWLP_HINSTANCE));
            WCHAR ClassName[256];
            GetClassNameW(hWnd, ClassName, sizeof(ClassName) / sizeof(WCHAR));
            GEngineLoop.DestroyEngineWindow(hWnd, hInstance, ClassName);
            if (UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine))
            {
                if (GEngineLoop.AppWindows.Num() == 0 || GEngineLoop.DefaultWindow == hWnd)
                {
                    EditorEngine->GetLevelEditor()->SaveConfig();
                }
                if (!EditorEngine->GetLevelEditor()->GetViewportClients(hWnd).IsEmpty())
                {
                    EditorEngine->RemoveWorld(EditorEngine->GetLevelEditor()->GetViewportClients(hWnd)[0]->World);
                }
                EditorEngine->GetLevelEditor()->RemoveViewportClients(hWnd);
            }
            if (!(GEngineLoop.AppWindows.Num() == 0 || GEngineLoop.DefaultWindow == hWnd))
            {
                break;
            }
        }
        //break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_SIZE:
            if (wParam != SIZE_MINIMIZED)
            {
                FEngineLoop::GraphicDevice.OnResize(hWnd);
                
                if (const UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine))
                {
                    if (SLevelEditor* LevelEditor = EditorEngine->GetLevelEditor())
                    {
                        const FVector2D ClientSize = FWindowsCursor::GetClientSize(hWnd);
                        LevelEditor->ResizeWindow(hWnd, ClientSize);
                        // for (std::shared_ptr<FEditorViewportClient>& ViewportClient : LevelEditor->GetViewports())
                        // {
                        //     FWindowData& WindowData = FEngineLoop::GraphicDevice.SwapChains[hWnd];
                        //     ViewportClient->ResizeViewport(WindowData.screenWidth, WindowData.screenHeight);
                        // }
                    }   
                }
            }
            ViewportTypePanel::GetInstance().OnResize(hWnd);
            GEngineLoop.UpdateUI(hWnd);
            break;
        default:
            GEngineLoop.AppMessageHandler->ProcessMessage(hWnd, Msg, wParam, lParam);
            return DefWindowProc(hWnd, Msg, wParam, lParam);
    }

    return 0;
}