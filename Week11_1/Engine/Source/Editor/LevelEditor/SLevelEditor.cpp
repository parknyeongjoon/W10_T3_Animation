#include "SLevelEditor.h"

#include "ImGuiManager.h"

#include "Slate/Widgets/Layout/SSplitter.h"
#include "Viewport.h"
#include "UnrealEd/EditorViewportClient.h"
#include "LaunchEngineLoop.h"
#include "WindowsCursor.h"
#include "Engine/World.h"
#include "UnrealEd/EditorPlayer.h"

void SLevelEditor::Initialize(UWorld* World, const HWND OwnerWindow)
{
    ActiveViewportWindow = OwnerWindow;
    ActiveViewportClientIndex = 0;
    for (size_t Index = 0; Index < 4; Index++)
    {
        const std::shared_ptr<FEditorViewportClient> EditorViewportClient = AddViewportClient<FEditorViewportClient>(OwnerWindow, World, Editor);
        EditorViewportClient->GetViewport()->ViewScreenLocation = static_cast<EViewScreenLocation>(Index);
    }

    const auto VSplitter = std::make_shared<SSplitterV>();
    VSplitter->Initialize(FRect(0, 0, WindowViewportDataMap[OwnerWindow].EditorWidth, WindowViewportDataMap[OwnerWindow].EditorHeight));
    WindowViewportDataMap[OwnerWindow].VSplitter = VSplitter;

    const auto HSplitter = std::make_shared<SSplitterH>();
    HSplitter->Initialize(FRect(0, 0, WindowViewportDataMap[OwnerWindow].EditorWidth, WindowViewportDataMap[OwnerWindow].EditorHeight));
    WindowViewportDataMap[OwnerWindow].HSplitter = HSplitter;

    LoadConfig();

    FSlateAppMessageHandler* Handler = GEngineLoop.GetAppMessageHandler();

    Handler->OnPIEModeStartDelegate.AddLambda([this]()
        {
            this->RegisterPIEInputDelegates();
        });

    Handler->OnPIEModeEndDelegate.AddLambda([this]()
        {
            this->RegisterEditorInputDelegates();
        });

    RegisterEditorInputDelegates();
}

void SLevelEditor::Tick(double DeltaTime)
{
    for (auto& [AppWnd, WindowViewportData] : WindowViewportDataMap)
    {
        for (std::shared_ptr<FEditorViewportClient>& ViewportClient : WindowViewportData.ViewportClients)
        {
            ViewportClient->Tick(DeltaTime);
        }
    }
}

void SLevelEditor::Release()
{
    WindowViewportDataMap.Empty();
}

// 렌더대상인 Render
template <typename T>
    requires std::derived_from<T, FViewportClient>
std::shared_ptr<T> SLevelEditor::AddViewportClient(HWND OwnerWindow, UWorld* World, const EViewportClientType Type)
{
    if (!WindowViewportDataMap.Contains(OwnerWindow))
    {
        WindowViewportDataMap.Add(OwnerWindow, FWindowViewportClientData());
        WindowViewportDataMap[OwnerWindow].EditorWidth = GEngineLoop.GraphicDevice.SwapChains[OwnerWindow].ScreenWidth;
        WindowViewportDataMap[OwnerWindow].EditorHeight = GEngineLoop.GraphicDevice.SwapChains[OwnerWindow].ScreenHeight;
        WindowViewportDataMap[OwnerWindow].ViewportClientType = Type;
    }
    
    std::shared_ptr<T> ViewportClient = std::make_shared<T>();
    ViewportClient->Initialize(OwnerWindow, WindowViewportDataMap[OwnerWindow].ViewportClients.Num(), World);

    WindowViewportDataMap[OwnerWindow].ViewportClients.Add(ViewportClient);
    return ViewportClient;
}

int SLevelEditor::GetNumViewportClientByType(const EViewportClientType Type)
{
    int count = 0;
    for (auto& [AppWnd, WindowViewportData] : WindowViewportDataMap)
    {
        if (WindowViewportData.ViewportClientType == Type)
        {
            count++;
        }
    }
    return count;
}

void SLevelEditor::RemoveViewportClient(const HWND OwnerWindow, const std::shared_ptr<FEditorViewportClient>& ViewportClient)
{
    if (!WindowViewportDataMap.Contains(OwnerWindow))
    {
        return;
    }
    
    // ViewportClient 소멸자에서 해줌.
    // ViewportClient->Release();
    
    FWindowViewportClientData& WindowViewportData = WindowViewportDataMap[OwnerWindow];
    WindowViewportData.ViewportClients.Remove(ViewportClient);
    if (WindowViewportData.ViewportClients.Num() == 0)
    {
        WindowViewportDataMap.Remove(OwnerWindow);
        
        ActiveViewportWindow = nullptr;
        ActiveViewportClientIndex = 0;
    }
    else
    {
        for (uint32 Index = 0; Index < WindowViewportData.ViewportClients.Num(); Index++)
        {
            WindowViewportDataMap[OwnerWindow].ViewportClients[Index]->SetViewportIndex(Index);
        }
        
        ActiveViewportWindow = WindowViewportDataMap.begin()->Key;
        ActiveViewportClientIndex = WindowViewportDataMap.begin()->Value.ActiveViewportIndex;
    }
}

void SLevelEditor::RemoveViewportClients(const HWND OwnerWindow)
{

    WindowViewportDataMap.Remove(OwnerWindow);
    if (WindowViewportDataMap.Num() > 0)
    {
        ActiveViewportWindow = WindowViewportDataMap.begin()->Key;
        ActiveViewportClientIndex = WindowViewportDataMap.begin()->Value.ActiveViewportIndex;
    }
    else
    {
        ActiveViewportWindow = nullptr;
        ActiveViewportClientIndex = 0;
    }
}

void SLevelEditor::SelectViewport(const HWND AppWnd, const FVector2D Point)
{
    if (!WindowViewportDataMap.Contains(AppWnd))
    {
        return;
    }

    FWindowViewportClientData& WindowViewportData = WindowViewportDataMap[AppWnd];
    std::shared_ptr<FEditorViewportClient> ActiveViewportClient = WindowViewportData.GetActiveViewportClient();
    
    for (uint32 Index = 0; Index < WindowViewportData.ViewportClients.Num(); Index++)
    {
        if (WindowViewportData.ViewportClients[Index]->GetViewport()->GetFSlateRect().Contains(Point))
        {
            FocusViewportClient(AppWnd, Index);
            break;
        }
    }
}

void SLevelEditor::ResizeWindow(const HWND AppWnd, const FVector2D ClientSize)
{
    if (!WindowViewportDataMap.Contains(AppWnd))
    {
        return;
    }

    FWindowViewportClientData& WindowViewportData = WindowViewportDataMap[AppWnd];

    WindowViewportData.EditorWidth = ClientSize.X;
    WindowViewportData.EditorHeight = ClientSize.Y;

    if (WindowViewportData.HSplitter)
    {
        WindowViewportData.HSplitter->Resize(WindowViewportData.EditorWidth, WindowViewportData.EditorHeight);
    }

    if (WindowViewportData.VSplitter)
    {
        WindowViewportData.VSplitter->Resize(WindowViewportData.EditorWidth, WindowViewportData.EditorHeight);
    }
    
    ResizeViewports(AppWnd);
}

void SLevelEditor::ResizeViewports(const HWND AppWnd)
{
    if (!WindowViewportDataMap.Contains(AppWnd))
    {
        return;
    }

    FWindowViewportClientData& WindowViewportData = WindowViewportDataMap[AppWnd];
    
    if (WindowViewportData.bMultiViewportMode)
    {
        for (const std::shared_ptr<FEditorViewportClient> EditorViewportClient : WindowViewportData.ViewportClients)
        {
            EditorViewportClient->ResizeViewport(
                WindowViewportData.VSplitter->SideLT->GetRect(), WindowViewportData.VSplitter->SideRB->GetRect(),
                WindowViewportData.HSplitter->SideLT->GetRect(), WindowViewportData.HSplitter->SideRB->GetRect());
        }
    }
    else
    {
        WindowViewportData.ViewportClients[WindowViewportData.ActiveViewportIndex]->ResizeViewport(
            FRect(0.0f, 0.0f, WindowViewportData.EditorWidth, WindowViewportData.EditorHeight));
    }
}

void SLevelEditor::SetEnableMultiViewport(const HWND AppWnd, const bool bIsEnable)
{
    if (WindowViewportDataMap[AppWnd].VSplitter != nullptr || WindowViewportDataMap[AppWnd].HSplitter != nullptr)
    {
        WindowViewportDataMap[AppWnd].bMultiViewportMode = bIsEnable;
        ResizeViewports(AppWnd);
    }
    else
    {
        WindowViewportDataMap[AppWnd].bMultiViewportMode = false;   
    }
}

bool SLevelEditor::IsMultiViewport(const HWND AppWnd)
{
    return WindowViewportDataMap[AppWnd].bMultiViewportMode;
}

void SLevelEditor::LoadConfig()
{
    const auto Config = ReadIniFile(IniFilePath);

    FWindowViewportClientData& WindowViewportData = WindowViewportDataMap[GEngineLoop.GetDefaultWindow()];
    const std::shared_ptr<FEditorViewportClient> ActiveViewportClient = WindowViewportData.GetActiveViewportClient();
    
    ActiveViewportClient->Pivot.X = GetValueFromConfig(Config, "OrthoPivotX", 0.0f);
    ActiveViewportClient->Pivot.Y = GetValueFromConfig(Config, "OrthoPivotY", 0.0f);
    ActiveViewportClient->Pivot.Z = GetValueFromConfig(Config, "OrthoPivotZ", 0.0f);
    ActiveViewportClient->OrthoSize = GetValueFromConfig(Config, "OrthoZoomSize", 10.0f);
    
    WindowViewportData.ActiveViewportIndex = GetValueFromConfig(Config, "ActiveViewportIndex", 0);
    WindowViewportData.bMultiViewportMode = GetValueFromConfig(Config, "bMutiView", false);
    for (size_t Index = 0; Index < 4; Index++)
    {
        WindowViewportData.ViewportClients[Index]->LoadConfig(Config);
    }
    
    if (WindowViewportData.HSplitter)
    {
        WindowViewportData.HSplitter->LoadConfig(Config);
    }
    if (WindowViewportData.VSplitter)
    {
        WindowViewportData.VSplitter->LoadConfig(Config);
    }

    for (auto& [AppWnd, WindowViewportData] : WindowViewportDataMap)
    {
        ResizeViewports(AppWnd);
    }
}

void SLevelEditor::SaveConfig()
{    
    TMap<FString, FString> config;

    FWindowViewportClientData& WindowViewportData = WindowViewportDataMap[GEngineLoop.GetDefaultWindow()];
    
    if (WindowViewportData.HSplitter)
        WindowViewportData.HSplitter->SaveConfig(config);
    if (WindowViewportData.VSplitter)
        WindowViewportData.VSplitter->SaveConfig(config);
    for (size_t i = 0; i < 4; i++)
    {
        WindowViewportData.ViewportClients[i]->SaveConfig(config);
    }
    std::shared_ptr<FEditorViewportClient> ActiveViewportClient = WindowViewportData.GetActiveViewportClient();
    ActiveViewportClient->SaveConfig(config);
    config["bMutiView"] = std::to_string(WindowViewportData.bMultiViewportMode);
    config["ActiveViewportIndex"] = std::to_string(ActiveViewportClient->GetViewportIndex());
    config["OrthoPivotX"] = std::to_string(ActiveViewportClient->Pivot.X);
    config["OrthoPivotY"] = std::to_string(ActiveViewportClient->Pivot.Y);
    config["OrthoPivotZ"] = std::to_string(ActiveViewportClient->Pivot.Z);
    config["OrthoZoomSize"] = std::to_string(ActiveViewportClient->OrthoSize);
    WriteIniFile(IniFilePath, config);
}

TMap<FString, FString> SLevelEditor::ReadIniFile(const FString& FilePath) const
{
    TMap<FString, FString> Config;
    std::ifstream File(*FilePath);
    std::string Line;

    while (std::getline(File, Line))
    {
        if (Line.empty() || Line[0] == '[' || Line[0] == ';')
        {
            continue;
        }

        std::istringstream SS(Line);
        std::string Key, Value;
        if (std::getline(SS, Key, '=') && std::getline(SS, Value))
        {
            Config[Key] = Value;
        }
    }
    return Config;
}

void SLevelEditor::WriteIniFile(const FString& FilePath, const TMap<FString, FString>& Config) const
{
    std::ofstream File(*FilePath);
    for (const auto& Pair : Config)
    {
        File << *Pair.Key << "=" << *Pair.Value << "\n";
    }
}

void SLevelEditor::RegisterEditorInputDelegates()
{
    FSlateAppMessageHandler* Handler = GEngineLoop.GetAppMessageHandler();

    // Clear current delegate functions
    for (const FDelegateHandle& Handle : InputDelegatesHandles)
    {
        Handler->OnKeyCharDelegate.Remove(Handle);
        Handler->OnKeyDownDelegate.Remove(Handle);
        Handler->OnKeyUpDelegate.Remove(Handle);

        Handler->OnMouseDownDelegate.Remove(Handle);
        Handler->OnMouseUpDelegate.Remove(Handle);
        Handler->OnMouseDoubleClickDelegate.Remove(Handle);
        Handler->OnMouseWheelDelegate.Remove(Handle);
        Handler->OnMouseMoveDelegate.Remove(Handle);

        Handler->OnRawMouseInputDelegate.Remove(Handle);
        Handler->OnRawKeyboardInputDelegate.Remove(Handle);
    }

    // Mouse Inputs
    {
        InputDelegatesHandles.Add(Handler->OnMouseDownDelegate.AddLambda([this](const FPointerEvent& InMouseEvent, const HWND AppWnd)
            {
                if (ImGuiManager::Get().GetWantCaptureMouse(AppWnd))
                {
                    return;
                }

                if (!WindowViewportDataMap.Contains(AppWnd))
                {
                    return;
                }

                UEditorPlayer* EditorPlayer = nullptr;
                if (const UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine))
                {
                    EditorPlayer = EditorEngine->GetEditorPlayer();
                }

                if (!EditorPlayer)
                {
                    return;
                }

                FWindowViewportClientData& WindowViewportData = WindowViewportDataMap[AppWnd];
                const std::shared_ptr<FEditorViewportClient> ActiveViewportClient = WindowViewportData.GetActiveViewportClient();

                switch (InMouseEvent.GetEffectingButton())  // NOLINT(clang-diagnostic-switch-enum)
                {
                    case EKeys::RightMouseButton:
                    {
                        if (!InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
                        {
                            FWindowsCursor::SetShowMouseCursor(false);
                            MousePinPosition = InMouseEvent.GetScreenSpacePosition();
                        }

                            if (ActiveViewportClient)
                            {
                                ActiveViewportClient->SetRightMouseDown(true);
                            }

                        break;
                    }
                    case EKeys::LeftMouseButton:
                    {
                        if(InMouseEvent.IsLeftAltDown() && InMouseEvent.IsControlDown())
                        {
                            EditorPlayer->MultiSelectingStart();
                        }

                        const FVector2D ClientPos = FWindowsCursor::GetClientPosition(AppWnd);
                        FVector PickPosition;
                        EditorPlayer->ScreenToViewSpace(ClientPos.X, ClientPos.Y, ActiveViewportClient->GetViewMatrix(), ActiveViewportClient->GetProjectionMatrix(), PickPosition);
                        
                        if (!EditorPlayer->PickGizmo(WindowViewportData.GetControlMode(), ActiveViewportClient->GetWorld(), PickPosition))
                        {
                            EditorPlayer->PickActor(ActiveViewportClient->GetWorld(), PickPosition);
                        }
                        break;
                    }
                    default:
                    {
                        break;
                    }
                }

                const FVector2D ClientPos = FWindowsCursor::GetClientPosition(AppWnd);
                SelectViewport(AppWnd, ClientPos);

                if (WindowViewportData.bMultiViewportMode)
                {
                    WindowViewportData.VSplitter->OnPressed(FPoint(ClientPos.X, ClientPos.Y));
                    WindowViewportData.HSplitter->OnPressed(FPoint(ClientPos.X, ClientPos.Y));
                }
            }
        ));

        InputDelegatesHandles.Add(Handler->OnMouseUpDelegate.AddLambda([this](const FPointerEvent& InMouseEvent, const HWND AppWnd)
            {
                if (ImGuiManager::Get().GetWantCaptureMouse(AppWnd))
                {
                    return;
                }

                if (!WindowViewportDataMap.Contains(AppWnd))
                {
                    return;
                }


                UEditorPlayer* EditorPlayer = nullptr;
                if (const UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine))
                {
                    EditorPlayer = EditorEngine->GetEditorPlayer();
                }

                if (!EditorPlayer)
                {
                    return;
                }

                FWindowViewportClientData& WindowViewportData = WindowViewportDataMap[AppWnd];
                const std::shared_ptr<FEditorViewportClient> ActiveViewportClient = WindowViewportData.GetActiveViewportClient();

                switch (InMouseEvent.GetEffectingButton())  // NOLINT(clang-diagnostic-switch-enum)
                {
                case EKeys::RightMouseButton:
                {
                    FWindowsCursor::SetShowMouseCursor(true);
                    FWindowsCursor::SetPosition(
                        static_cast<int32>(MousePinPosition.X),
                        static_cast<int32>(MousePinPosition.Y)
                    );


                    if (ActiveViewportClient)
                    {
                        ActiveViewportClient->SetRightMouseDown(false);
                    }

                    // @todo ImGui로 피킹된 액터의 옵션 메뉴 표기
                    return;
                }
                case EKeys::LeftMouseButton:
                {
                    if (WindowViewportData.VSplitter)
                    {
                        WindowViewportData.VSplitter->OnReleased();
                    }
                    if (WindowViewportData.HSplitter)
                    {
                        WindowViewportData.HSplitter->OnReleased();
                    }

                    EditorPlayer->SetAlreadyDup(false);
                    if (EditorPlayer->GetMultiSelecting())
                    {
                        EditorPlayer->MultiSelectingEnd(WindowViewportData.GetActiveViewportClient()->GetWorld());
                    }
                    else
                    {
                        WindowViewportData.GetActiveViewportClient()->GetWorld()->SetPickingGizmo(nullptr);
                    }
                    return;
                }
                default:
                    return;
                }
            }
        ));

        InputDelegatesHandles.Add(Handler->OnMouseMoveDelegate.AddLambda([this](const FPointerEvent& InMouseEvent, const HWND AppWnd)
            {
                if (ImGuiManager::Get().GetWantCaptureMouse(AppWnd))
                {
                    return;
                }

                if (!WindowViewportDataMap.Contains(AppWnd))
                {
                    return;
                }

                UEditorPlayer* EditorPlayer = nullptr;
                if (const UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine))
                {
                    EditorPlayer = EditorEngine->GetEditorPlayer();
                }

                if (!EditorPlayer)
                {
                    return;
                }

                FWindowViewportClientData& WindowViewportData = WindowViewportDataMap[AppWnd];

                // @todo ImGui 패널 Slate 적용 후 대응
                // Splitter 움직임 로직
                if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
                {
                    const auto& [DeltaX, DeltaY] = InMouseEvent.GetCursorDelta();

                    bool bSplitterDragging = false;
                    if (WindowViewportData.VSplitter && WindowViewportData.VSplitter->IsSplitterPressed())
                    {
                        WindowViewportData.VSplitter->OnDrag(FPoint(DeltaX, DeltaY));
                        bSplitterDragging = true;
                    }
                    if (WindowViewportData.HSplitter && WindowViewportData.HSplitter->IsSplitterPressed())
                    {
                        WindowViewportData.HSplitter->OnDrag(FPoint(DeltaX, DeltaY));
                        bSplitterDragging = true;
                    }

                    if (bSplitterDragging)
                    {
                        ResizeViewports(AppWnd);
                    }
                }

                // @todo ImGui 패널 Slate 적용 후 대응
                // 커서 변경 로직
                if (WindowViewportData.bMultiViewportMode && !InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton) && !InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
                {
                    // @todo ImGui 패널 Slate 적용 후 커서가 Viewport 위에 있을때만 ECursorType::Crosshair로 표기
                    auto CursorType = ECursorType::Arrow;

                    FVector2D ClientPos = FWindowsCursor::GetClientPosition(AppWnd);
                    const bool bIsVerticalHovered = WindowViewportData.VSplitter ? WindowViewportData.VSplitter->IsHover(FPoint{ ClientPos.X, ClientPos.Y }) : false;
                    const bool bIsHorizontalHovered = WindowViewportData.HSplitter ? WindowViewportData.HSplitter->IsHover(FPoint{ ClientPos.X, ClientPos.Y }) : false;

                    if (bIsHorizontalHovered && bIsVerticalHovered)
                    {
                        CursorType = ECursorType::ResizeAll;
                    }
                    if (bIsHorizontalHovered)
                    {
                        CursorType = ECursorType::ResizeLeftRight;
                    }
                    else if (bIsVerticalHovered)
                    {
                        CursorType = ECursorType::ResizeUpDown;
                    }

                    FWindowsCursor::SetMouseCursor(CursorType);
                }

                if (EditorPlayer->GetMultiSelecting())
                {
                    EditorPlayer->MakeMulitRect();
                }

                EditorPlayer->PickedObjControl(WindowViewportData.GetControlMode(), WindowViewportData.GetCoordiMode(), WindowViewportData.GetActiveViewportClient()->GetWorld());
            }
        ));

        InputDelegatesHandles.Add(Handler->OnMouseWheelDelegate.AddLambda([this](const FPointerEvent& InMouseEvent, const HWND AppWnd)
            {
                if (ImGuiManager::Get().GetWantCaptureMouse(AppWnd))
                {
                    return;
                }

                if (!WindowViewportDataMap.Contains(AppWnd))
                {
                    return;
                }

                FWindowViewportClientData& WindowViewportClientData = WindowViewportDataMap[AppWnd];
                const std::shared_ptr<FEditorViewportClient> ActiveViewportClient = WindowViewportClientData.GetActiveViewportClient();

                // 뷰포트에서 앞뒤 방향으로 화면 이동
                if (ActiveViewportClient->IsPerspective())
                {
                    if (!InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
                    {
                        const FVector CameraLoc = ActiveViewportClient->ViewTransformPerspective.GetLocation();
                        const FVector CameraForward = ActiveViewportClient->ViewTransformPerspective.GetForwardVector();
                        ActiveViewportClient->ViewTransformPerspective.SetLocation(CameraLoc + CameraForward * InMouseEvent.GetWheelDelta() * 50.0f);
                    }
                }
                else
                {
                    FEditorViewportClient::SetOrthoSize(-InMouseEvent.GetWheelDelta());
                }
            }
        ));
    }

    // Keyboard Inputs
    {
        InputDelegatesHandles.Add(Handler->OnKeyDownDelegate.AddLambda([this](const FKeyEvent& InKeyEvent, const HWND AppWnd)
            {
                if (ImGuiManager::Get().GetWantCaptureKeyboard(AppWnd))
                {
                    return;
                }

                if (!WindowViewportDataMap.Contains(AppWnd))
                {
                    return;
                }

                FWindowViewportClientData& WindowViewportClientData = WindowViewportDataMap[AppWnd];
                WindowViewportClientData.GetActiveViewportClient()->InputKey(AppWnd, InKeyEvent);

                if (GetKeyState(VK_RBUTTON) & 0x8000)
                {
                    return;
                }

                if (InKeyEvent.GetInputEvent() != IE_Pressed)
                {
                    return;
                }

                switch (InKeyEvent.GetCharacter())
                {
                case 'Q':
                    {
                        break;
                    }
                case 'W':
                    {
                        WindowViewportClientData.SetMode(CM_TRANSLATION);
                        break;
                    }
                case 'E':
                    {
                        WindowViewportClientData.SetMode(CM_ROTATION);
                        break;
                    }
                case 'R':
                    {
                        WindowViewportClientData.SetMode(CM_SCALE);
                        break;
                    }
                default:
                    break;
                }
            }
        ));

        InputDelegatesHandles.Add(Handler->OnKeyUpDelegate.AddLambda([this](const FKeyEvent& InKeyEvent, const HWND AppWnd)
            {
                if (ImGuiManager::Get().GetWantCaptureKeyboard(AppWnd))
                {
                    return;
                }

                if (!WindowViewportDataMap.Contains(AppWnd))
                {
                    return;
                }

                FWindowViewportClientData WindowViewportClientData = WindowViewportDataMap[AppWnd];
                WindowViewportClientData.GetActiveViewportClient()->InputKey(AppWnd, InKeyEvent);
            }
        ));
    }

    // Raw Inputs
    {
        InputDelegatesHandles.Add(Handler->OnRawMouseInputDelegate.AddLambda([this](const FPointerEvent& InMouseEvent, const HWND AppWnd)
            {
                if (ImGuiManager::Get().GetWantCaptureMouse(AppWnd))
                {
                    return;
                }

                if (!WindowViewportDataMap.Contains(AppWnd))
                {
                    return;
                }

                FWindowViewportClientData& WindowViewportClientData = WindowViewportDataMap[AppWnd];
                const std::shared_ptr<FEditorViewportClient> ActiveViewportClient = WindowViewportClientData.GetActiveViewportClient();

                // Mouse Move 이벤트 일때만 실행
                if (InMouseEvent.GetInputEvent() == IE_Axis
                    && InMouseEvent.GetEffectingButton() == EKeys::Invalid)
                {
                    // 에디터 카메라 이동 로직
                    if (!InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton)
                        && InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
                    {
                        ActiveViewportClient->MouseMove(InMouseEvent);
                    }
                    // @todo Gizmo 컨트롤 로직 확인할 것
                }
                // 마우스 휠 이벤트
                else if (InMouseEvent.GetEffectingButton() == EKeys::MouseWheelAxis)
                {
                    // 카메라 속도 조절
                    if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton) && ActiveViewportClient->IsPerspective())
                    {
                        const float CurrentSpeed = ActiveViewportClient->GetCameraSpeedScalar();
                        const float Adjustment = FMath::Sign(InMouseEvent.GetWheelDelta()) * FMath::Loge(CurrentSpeed + 1.0f) * 0.5f;

                        ActiveViewportClient->SetCameraSpeed(CurrentSpeed + Adjustment);
                    }
                }
            }
        ));
    }
}

void SLevelEditor::RegisterPIEInputDelegates()
{
    FSlateAppMessageHandler* Handler = GEngineLoop.GetAppMessageHandler();

    // Clear current delegate functions
    for (const FDelegateHandle& Handle : InputDelegatesHandles)
    {
        Handler->OnKeyCharDelegate.Remove(Handle);
        Handler->OnKeyDownDelegate.Remove(Handle);
        Handler->OnKeyUpDelegate.Remove(Handle);
        Handler->OnMouseDownDelegate.Remove(Handle);
        Handler->OnMouseUpDelegate.Remove(Handle);
        Handler->OnMouseDoubleClickDelegate.Remove(Handle);
        Handler->OnMouseWheelDelegate.Remove(Handle);
        Handler->OnMouseMoveDelegate.Remove(Handle);
        Handler->OnRawMouseInputDelegate.Remove(Handle);
        Handler->OnRawKeyboardInputDelegate.Remove(Handle);
    }

    // Add Delegate functions in PIE mode
    // ...
}
