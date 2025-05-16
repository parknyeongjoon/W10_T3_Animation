#include "EditorViewportClient.h"

#include <algorithm>

#include "EditorPlayer.h"
#include "ImGUI/imgui.h"

#include "Math/JungleMath.h"
#include "LaunchEngineLoop.h"
#include "Viewport.h"
#include "Engine/FLoaderOBJ.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraComponent.h"
#include "Components/LightComponents/DirectionalLightComponent.h"
#include "Components/LightComponents/SpotLightComponent.h"
#include "Engine/FEditorStateManager.h"
#include "GameFramework/PlayerController.h"
#include "LevelEditor/SLevelEditor.h"
#include "SlateCore/Layout/SlateRect.h"

FVector FEditorViewportClient::Pivot = FVector(0.0f, 0.0f, 0.0f);
float FEditorViewportClient::OrthoSize = 10.0f;

FEditorViewportClient::FEditorViewportClient()
    : GridSize(10)
    , Viewport(nullptr)
    , ViewportIndex(0)
    , ViewportType(LVT_Perspective)
    , ShowFlag(31 | (1ULL << 10))
    , ViewMode(VMI_Lit_Phong)
{
}

FEditorViewportClient::~FEditorViewportClient()
{
    Release();
}

void FEditorViewportClient::Draw(FViewport* Viewport)
{
}

UWorld* FEditorViewportClient::GetWorld() const
{
    return World;
}

void FEditorViewportClient::Initialize(const HWND InOwnerWindow, const uint32 InViewportIndex, UWorld* World)
{
    SetOwner(InOwnerWindow);
    SetWorld(World);
    ViewportIndex = InViewportIndex;
    
    ViewTransformPerspective.SetLocation(FVector(8.0f, 8.0f, 8.f));
    ViewTransformPerspective.SetRotation(FVector(0.0f, 45.0f, -135.0f));
    Viewport = new FViewport();
    const FWindowData& WindowData = FEngineLoop::GraphicDevice.SwapChains[InOwnerWindow];
    ResizeViewport(FRect(0, 0, WindowData.ScreenWidth, WindowData.ScreenHeight));
}

void FEditorViewportClient::Tick(const float DeltaTime)
{
    if (GetWorld()->WorldType == EWorldType::Editor || GetWorld()->WorldType == EWorldType::EditorPreview)
    {
        UpdateEditorCameraMovement(DeltaTime);
    }
    
    UpdateViewMatrix();
    UpdateProjectionMatrix();
    UpdateCascadeShadowArea();
}

void FEditorViewportClient::Release() const
{
    delete Viewport;
}

void FEditorViewportClient::UpdateEditorCameraMovement(const float DeltaTime)
{
    if (CameraInputPressedKeys.Contains(EKeys::A))
    {
        CameraMoveRight(-100.f * DeltaTime);
    }

    if (CameraInputPressedKeys.Contains(EKeys::D))
    {
        CameraMoveRight(100.f * DeltaTime);
    }

    if (CameraInputPressedKeys.Contains(EKeys::W))
    {
        CameraMoveForward(100.f * DeltaTime);
    }

    if (CameraInputPressedKeys.Contains(EKeys::S))
    {
        CameraMoveForward(-100.f * DeltaTime);
    }

    if (CameraInputPressedKeys.Contains(EKeys::E))
    {
        CameraMoveUp(100.f * DeltaTime);
    }

    if (CameraInputPressedKeys.Contains(EKeys::Q))
    {
        CameraMoveUp(-100.f * DeltaTime);
    }
}

void FEditorViewportClient::InputKey(const HWND AppWnd, const FKeyEvent& InKeyEvent)
{
    // TODO: 나중에 InKeyEvent.GetKey();로 가져오는걸로 수정하기
    // TODO: 나중에 PIEViewportClient에서 처리하는걸로 수정하기
    if (GetWorld()->WorldType == EWorldType::PIE)
    {
        // PIE 모드 → 게임 플레이 입력 처리
        if (UWorld* PlayWorld = GetWorld())
        {
            // 첫 번째 플레이어 컨트롤러에게 전달
            if (APlayerController* PC = PlayWorld->GetPlayerController())
            {
                // FKeyEvent → EKeys, EInputEvent로 변환 후 호출

                switch (InKeyEvent.GetCharacter())
                {
                case 'A':
                {
                    if (InKeyEvent.GetInputEvent() == IE_Pressed)
                    {
                        PC->InputKey(EKeys::A, IE_Pressed);
                    }
                    else if (InKeyEvent.GetInputEvent() == IE_Released)
                    {
                        PC->InputKey(EKeys::A, IE_Released);
                    }
                    break;
                }
                case 'D':
                {
                    if (InKeyEvent.GetInputEvent() == IE_Pressed)
                    {
                        PC->InputKey(EKeys::D, IE_Pressed);
                    }
                    else if (InKeyEvent.GetInputEvent() == IE_Released)
                    {
                        PC->InputKey(EKeys::D, IE_Released);
                    }
                    break;
                }
                case 'W':
                {
                    if (InKeyEvent.GetInputEvent() == IE_Pressed)
                    {
                        PC->InputKey(EKeys::W, IE_Pressed);
                    }
                    else if (InKeyEvent.GetInputEvent() == IE_Released)
                    {
                        PC->InputKey(EKeys::W, IE_Released);
                    }
                    break;
                }
                case 'S':
                {
                    if (InKeyEvent.GetInputEvent() == IE_Pressed)
                    {
                        PC->InputKey(EKeys::S, IE_Pressed);
                    }
                    else if (InKeyEvent.GetInputEvent() == IE_Released)
                    {
                        PC->InputKey(EKeys::S, IE_Released);
                    }
                    break;
                }
                case 'Z':
                    {
                        if (InKeyEvent.GetInputEvent() == IE_Pressed)
                        {
                            PC->InputKey(EKeys::Z, IE_Pressed);
                        }
                        else if (InKeyEvent.GetInputEvent() == IE_Released)
                        {
                            PC->InputKey(EKeys::Z, IE_Released);
                        }
                        break;
                    }
                case 'X':
                    {
                        if (InKeyEvent.GetInputEvent() == IE_Pressed)
                        {
                            PC->InputKey(EKeys::X, IE_Pressed);
                        }
                        else if (InKeyEvent.GetInputEvent() == IE_Released)
                        {
                            PC->InputKey(EKeys::X, IE_Released);
                        }
                        break;
                    }
                case 'C':
                    {
                        if (InKeyEvent.GetInputEvent() == IE_Pressed)
                        {
                            PC->InputKey(EKeys::C, IE_Pressed);
                        }
                        else if (InKeyEvent.GetInputEvent() == IE_Released)
                        {
                            PC->InputKey(EKeys::C, IE_Released);
                        }
                        break;
                    }
                }
            }
        }
    }
    else
    {
        // 마우스 우클릭이 되었을때만 실행되는 함수
        if (bRightMouseDown)
        {
            switch (InKeyEvent.GetCharacter())
            {
                case 'W':
                case 'A':
                case 'S':
                case 'D':
                case 'Q':
                case 'E':
                {
                    UpdateCameraInputKeyState(InKeyEvent);
                    break;
                }
                default:
                {
                    break;
                }
            }
        }
        else
        {
            CameraInputPressedKeys.Empty();

            /* @todo 뷰포트와 플레이어 중 조작 모드 누가 관리할지 정하기
            // 마우스 우클릭 상태가 아닌 경우 - 변환 모드 설정
            UEditorPlayer* EdPlayer = CastChecked<UEditorEngine>(GEngine)->GetEditorPlayer();
            switch (InKeyEvent.GetCharacter())
            {
            case 'W':
            {
                if (InKeyEvent.GetInputEvent() == IE_Pressed)
                {
                    EdPlayer->SetMode(CM_TRANSLATION);
                }
                break;
            }
            case 'E':
            {
                if (InKeyEvent.GetInputEvent() == IE_Pressed)
                {
                    EdPlayer->SetMode(CM_ROTATION);
                }
                break;
            }
            case 'R':
            {
                if (InKeyEvent.GetInputEvent() == IE_Pressed)
                {
                    EdPlayer->SetMode(CM_SCALE);
                }
                break;
            }
            default:
                break;
            }
            */

            if (InKeyEvent.GetInputEvent() == IE_Pressed)
            {
                switch (InKeyEvent.GetCharacter())
                {
                case 'F':
                {
                    TSet<AActor*> SelectedActors = GetWorld()->GetSelectedActors();
                    if (!SelectedActors.IsEmpty())
                    {
                        if (AActor* PickedActor = *SelectedActors.begin())
                        {
                            FViewportCameraTransform& ViewTransform = ViewTransformPerspective;
                            ViewTransform.SetLocation(
                                // TODO: 10.0f 대신, 정점의 min, max의 거리를 구해서 하면 좋을 듯
                                PickedActor->GetActorLocation() - (ViewTransform.GetForwardVector() * 10.0f)
                            );
                        }
                    }
                    break;
                }
                case 'M':
                {
                    if (UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine))
                    {
                        FEngineLoop::GraphicDevice.OnResize(OwnerWindow);
                        SLevelEditor* LevelEditor = EditorEngine->GetLevelEditor();
                        LevelEditor->SetEnableMultiViewport(AppWnd, !LevelEditor->IsMultiViewport(AppWnd));
                    }
                    break;
                }
                case 'D':
                {
                    if (PressedKeys.Contains(EKeys::LeftControl))
                    {
                        GetWorld()->DuplicateSelectedActors();
                    }
                }
                default:
                    break;
                }
            }
        }

        // 일반적인 단일 키 이벤트
        if (InKeyEvent.GetInputEvent() == IE_Pressed)
        {
            // Virtual Key
            UEditorEngine* EditorEngine = CastChecked<UEditorEngine>(GEngine);
            switch (InKeyEvent.GetKeyCode())
            {
            case VK_DELETE:
                {
                    auto SelectedActors = GetWorld()->GetSelectedActors();
                    if (!SelectedActors.IsEmpty())
                    {
                        // @todo 선택된 Component가 있는 경우 Component를 지울 것
                        //if (SelectedActors.Num() == 1)
                        //{
                        //}
                        //else
                        //{
                            for (AActor* Actor : SelectedActors)
                            {
                                UE_LOG(LogLevel::Display, "Delete Actor - %s", *Actor->GetName());
                                Actor->Destroy();
                            }
                        //}
                    }
                    GetWorld()->ClearSelectedActors();
                    break;
                }
            case VK_SPACE:
            {
                EditorEngine->GetLevelEditor()->GetActiveViewportClientData().AddControlMode();
                break;
            }
            case VK_LCONTROL:
            {
                PressedKeys.Add(EKeys::LeftControl);
                break;
            }
            default:
                break;
            }
        }
    }
}

void FEditorViewportClient::UpdateCameraInputKeyState(const FKeyEvent& InKeyEvent)
{
    const EKeys::Type& Key = InKeyEvent.GetKey();
    switch (InKeyEvent.GetInputEvent())
    {
        case IE_Pressed:
        {
            CameraInputPressedKeys.Add(Key);
            break;
        }
        case IE_Released:
        {
            CameraInputPressedKeys.Remove(Key);
            break;
        }
        default:
        {
            break;
        }
    }
}

void FEditorViewportClient::MouseMove(const FPointerEvent& InMouseEvent)
{
    const auto& [DeltaX, DeltaY] = InMouseEvent.GetCursorDelta();

    // 마우스 우클릭이 되었을때만 실행되는 함수
    // @todo bRightMouseDown와 InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton) 둘 중 무엇을 사용하는 게 좋을지? (FKeyEvent는 IsMouseButtonDown()이 없음)
    if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
    {
        // Yaw(좌우 회전) 및 Pitch(상하 회전) 값 변경
        if (IsPerspective())
        {
            CameraRotateYaw(DeltaX * 0.1f);  // X 이동에 따라 좌우 회전
            CameraRotatePitch(DeltaY * 0.1f);  // Y 이동에 따라 상하 회전
        }
        else
        {
            PivotMoveRight(DeltaX);
            PivotMoveUp(DeltaY);
        }
    }
}

void FEditorViewportClient::ResizeViewport(FRect Top, FRect Bottom, FRect Left, FRect Right)
{
    if (Viewport)
    { 
        Viewport->ResizeViewport(Top, Bottom, Left, Right);    
    }
    else
    {
        UE_LOG(LogLevel::Error, "Viewport is nullptr");
    }
    
    float Width = Viewport->GetFSlateRect().Width;
    float Height = Viewport->GetFSlateRect().Height;
    AspectRatio = Width / Height;

    UpdateProjectionMatrix();
    UpdateViewMatrix();
}

void FEditorViewportClient::ResizeViewport(FRect InRect)
{
    if (Viewport)
    { 
        Viewport->ResizeViewport(InRect);    
    }
    else
    {
        UE_LOG(LogLevel::Error, "Viewport is nullptr");
    }
    
    float Width = Viewport->GetFSlateRect().Width;
    float Height = Viewport->GetFSlateRect().Height;
    AspectRatio = Width / Height;

    UpdateProjectionMatrix();
    UpdateViewMatrix();
}

bool FEditorViewportClient::IsSelected(const FVector2D Point) const
{
    const float TopLeftX = Viewport->GetFSlateRect().LeftTopX;
    const float TopLeftY = Viewport->GetFSlateRect().LeftTopY;
    const float Width = Viewport->GetFSlateRect().Width;
    const float Height = Viewport->GetFSlateRect().Height;

    if (Point.X >= TopLeftX && Point.X <= TopLeftX + Width &&
        Point.Y >= TopLeftY && Point.Y <= TopLeftY + Height)
    {
        return true;
    }
    return false;
}

const D3D11_VIEWPORT& FEditorViewportClient::GetD3DViewport() const
{
    return Viewport->GetViewport();
}

void FEditorViewportClient::CalculateCascadeSplits(const float NearClip, const float FarClip)
{
    // 지수 분할 방식

    for (int i = 0; i < CASCADE_COUNT; i++)
    {
        constexpr float Lambda = 0.75f;
        const float P = static_cast<float>(i + 1) / static_cast<float>(CASCADE_COUNT);
        
        // 균등 분할 값
        const float UniformSplit = NearClip + (FarClip - NearClip) * P;
        
        // 지수 분할 값 (로그 스케일)
        const float ExponentialSplit = NearClip * pow(FarClip / NearClip, P);
        
        // 람다를 사용하여 두 방식을 혼합
        CascadeSplits[i] = Lambda * ExponentialSplit + (1 - Lambda) * UniformSplit;
    }
}

void FEditorViewportClient::CalculateFrustumCorners(const UINT CascadeIndex)
{
    const float PrevSplitDist = CascadeIndex == 0 ? GetNearClip() : CascadeSplits[CascadeIndex - 1];
    const float SplitDist = CascadeSplits[CascadeIndex];
    
    // 카메라 공간에서 절두체 코너 계산
    // 근평면 4개 코너 + 원평면 4개 코너
    CalculateFrustumCornersInCameraSpace(PrevSplitDist, SplitDist, CascadeIndex);
    
    // 월드 공간으로 변환
    const FMatrix CameraToWorldMatrix = FMatrix::Inverse(GetViewMatrix());
    for (int i = 0; i < 8; i++)
    {
        FVector4 worldCorner = CameraToWorldMatrix.TransformFVector4(FVector4(CascadeCorners[CascadeIndex][i], 1.0f));
        CascadeCorners[CascadeIndex][i] = FVector(worldCorner.X, worldCorner.Y, worldCorner.Z);
    }
}

void FEditorViewportClient::CalculateFrustumCornersInCameraSpace(const float NearDist, const float FarDist, const int CascadeIndex) {
    // 카메라 투영 행렬에서 필요한 값들을 추출
    float Fov = GetViewFOV();  // 시야각(라디안)
    Fov = FMath::DegreesToRadians(Fov);
    
    // 근평면과 원평면의 높이와 너비 계산
    const float NearHeight = 2.0f * tan(Fov * 0.5f) * NearDist;
    const float NearWidth = NearHeight * AspectRatio;
    const float FarHeight = 2.0f * tan(Fov * 0.5f) * FarDist;
    const float FarWidth = FarHeight * AspectRatio;
    
    // 근평면의 4개 모서리 계산 (카메라 공간)
    // 왼쪽 아래
    CascadeCorners[CascadeIndex][0] = FVector(-NearWidth * 0.5f, -NearHeight * 0.5f, NearDist);
    // 오른쪽 아래
    CascadeCorners[CascadeIndex][1] = FVector(NearWidth * 0.5f, -NearHeight * 0.5f, NearDist);
    // 오른쪽 위
    CascadeCorners[CascadeIndex][2] = FVector(NearWidth * 0.5f, NearHeight * 0.5f, NearDist);
    // 왼쪽 위
    CascadeCorners[CascadeIndex][3] = FVector(-NearWidth * 0.5f, NearHeight * 0.5f, NearDist);
    
    // 원평면의 4개 모서리 계산 (카메라 공간)
    // 왼쪽 아래
    CascadeCorners[CascadeIndex][4] = FVector(-FarWidth * 0.5f, -FarHeight * 0.5f, FarDist);
    // 오른쪽 아래
    CascadeCorners[CascadeIndex][5] = FVector(FarWidth * 0.5f, -FarHeight * 0.5f, FarDist);
    // 오른쪽 위
    CascadeCorners[CascadeIndex][6] = FVector(FarWidth * 0.5f, FarHeight * 0.5f, FarDist);
    // 왼쪽 위
    CascadeCorners[CascadeIndex][7] = FVector(-FarWidth * 0.5f, FarHeight * 0.5f, FarDist);
}

void FEditorViewportClient::UpdateCascadeShadowArea()
{
    CalculateCascadeSplits(GetNearClip(), GetFarClip());
    for (int i=0; i<CASCADE_COUNT; i++)
    {
        CalculateFrustumCorners(i);
    }
}

void FEditorViewportClient::CameraMoveForward(const float Value)
{
    if (!OverrideComponent)
    {
        if (IsPerspective())
        {
            FVector CurCameraLoc = ViewTransformPerspective.GetLocation();
            CurCameraLoc = CurCameraLoc + ViewTransformPerspective.GetForwardVector() * GetCameraSpeedScalar() * Value;
            ViewTransformPerspective.SetLocation(CurCameraLoc);
        }
        else
        {
            Pivot.X += Value * 0.1f;
        }
    }
}

void FEditorViewportClient::CameraMoveRight(const float Value)
{
    if (!OverrideComponent)
    {
        if (IsPerspective())
        {
            FVector CurCameraLoc = ViewTransformPerspective.GetLocation();
            CurCameraLoc = CurCameraLoc + ViewTransformPerspective.GetRightVector() * GetCameraSpeedScalar() * Value;
            ViewTransformPerspective.SetLocation(CurCameraLoc);
        }
        else
        {
            Pivot.Y += Value * 0.1f;
        }
    }
}

void FEditorViewportClient::CameraMoveUp(const float Value)
{
    if (!OverrideComponent)
    {
        if (IsPerspective())
        {
            FVector CurCameraLoc = ViewTransformPerspective.GetLocation();
            CurCameraLoc.Z = CurCameraLoc.Z + GetCameraSpeedScalar() * Value;
            ViewTransformPerspective.SetLocation(CurCameraLoc);
        }
        else
        {
            Pivot.Z += Value * 0.1f;
        }
    }
}

void FEditorViewportClient::CameraRotateYaw(float Value)
{
    if (!OverrideComponent)
    {
        FVector CurCameraRot = ViewTransformPerspective.GetRotation();
        CurCameraRot.Z += Value;
        ViewTransformPerspective.SetRotation(CurCameraRot);
    }
}

void FEditorViewportClient::CameraRotatePitch(const float Value)
{
    if (!OverrideComponent)
    {
        FVector curCameraRot = ViewTransformPerspective.GetRotation();
        curCameraRot.Y += Value;
        curCameraRot.Y = FMath::Max(curCameraRot.Y, -89.0f);
        curCameraRot.Y = FMath::Min(curCameraRot.Y, 89.0f);
        ViewTransformPerspective.SetRotation(curCameraRot);
    }
}

void FEditorViewportClient::PivotMoveRight(const float Value)
{
    if (!OverrideComponent)
    {
        Pivot = Pivot + ViewTransformOrthographic.GetRightVector() * Value * -0.05f;
    }
}

void FEditorViewportClient::PivotMoveUp(const float Value)
{
    if (!OverrideComponent)
    {
        Pivot = Pivot + ViewTransformOrthographic.GetUpVector() * Value * 0.05f;
    }
}

void FEditorViewportClient::UpdateViewMatrix()
{
    if (!OverrideComponent)
    {
        if (IsPerspective())
        {
            NearPlane = 0.1f;
            FarPlane = 1000.f;
            View = JungleMath::CreateViewMatrix(ViewTransformPerspective.GetLocation(),
                ViewTransformPerspective.GetLocation() + ViewTransformPerspective.GetForwardVector(),
                FVector{ 0.0f,0.0f, 1.0f }
            );
        }
        else
        {
            UpdateOrthoCameraLoc();
            if (ViewportType == LVT_OrthoXY || ViewportType == LVT_OrthoNegativeXY)
            {
                View = JungleMath::CreateViewMatrix(ViewTransformOrthographic.GetLocation(),
                    Pivot, FVector(0.0f, -1.0f, 0.0f)
                );
            }
            else
            {
                View = JungleMath::CreateViewMatrix(ViewTransformOrthographic.GetLocation(),
                    Pivot, FVector(0.0f, 0.0f, 1.0f)
                );
            }
        }
    }
    else
    {
        if (const USpotLightComponent* SpotLight = Cast<USpotLightComponent>(OverrideComponent))
        {
            View = SpotLight->GetViewMatrix();
        }
        if (const UDirectionalLightComponent* DirectionalLight = Cast<UDirectionalLightComponent>(OverrideComponent))
        {
            View = DirectionalLight->GetViewMatrix();
        }
        if (const UCameraComponent* PlayerCamera = Cast<UCameraComponent>(OverrideComponent))
        {
            View = PlayerCamera->GetViewMatrix();
        }
    }
}

void FEditorViewportClient::UpdateProjectionMatrix()
{
    if (!OverrideComponent)
    {
        if (IsPerspective())
        {
            Projection = JungleMath::CreateProjectionMatrix(
                ViewFOV * (PI / 180.0f),
                GetViewport()->GetFSlateRect().Width / GetViewport()->GetFSlateRect().Height,
                NearPlane,
                FarPlane
            );
        }
        else
        {
            // 오쏘그래픽 너비는 줌 값과 가로세로 비율에 따라 결정됩니다.
            const float OrthoWidth = OrthoSize * AspectRatio;
            const float OrthoHeight = OrthoSize;

            // 오쏘그래픽 투영 행렬 생성 (nearPlane, farPlane 은 기존 값 사용)
            Projection = JungleMath::CreateOrthoProjectionMatrix(
                OrthoWidth,
                OrthoHeight,
                NearPlane,
                FarPlane
            );
        }
    }
    else
    {
        if (const USpotLightComponent* SpotLight = Cast<USpotLightComponent>(OverrideComponent))
        {
            Projection = SpotLight->GetProjectionMatrix();
        }
        if (const UDirectionalLightComponent* DirectionalLight = Cast<UDirectionalLightComponent>(OverrideComponent))
        {
            Projection = DirectionalLight->GetProjectionMatrix();
        }
        if (UCameraComponent* PlayerCamera = Cast<UCameraComponent>(OverrideComponent))
        {
            Projection = JungleMath::CreateProjectionMatrix(
                    FMath::DegreesToRadians(PlayerCamera->GetFOV()),
                    AspectRatio,
                    PlayerCamera->GetNearClip(),
                    PlayerCamera->GetFarClip()
            );
        }
    }
}

bool FEditorViewportClient::IsOrtho() const
{
    return !IsPerspective();
}

bool FEditorViewportClient::IsPerspective() const
{
    return (GetViewportType() == LVT_Perspective);
}

ELevelViewportType FEditorViewportClient::GetViewportType() const
{
    ELevelViewportType EffectiveViewportType = ViewportType;
    if (EffectiveViewportType == LVT_None)
    {
        EffectiveViewportType = LVT_Perspective;
    }
    //if (bUseControllingActorViewInfo)
    //{
    //    EffectiveViewportType = (ControllingActorViewInfo.ProjectionMode == ECameraProjectionMode::Perspective) ? LVT_Perspective : LVT_OrthoFreelook;
    //}
    return EffectiveViewportType;
}

void FEditorViewportClient::SetViewportType(const ELevelViewportType InViewportType)
{
    ViewportType = InViewportType;
    //ApplyViewMode(GetViewMode(), IsPerspective(), EngineShowFlags);

    //// We might have changed to an orthographic viewport; if so, update any viewport links
    //UpdateLinkedOrthoViewports(true);

    //Invalidate();
}

void FEditorViewportClient::UpdateOrthoCameraLoc()
{
    const float FarDistance = FarPlane * 0.5f;
    switch (ViewportType)
    {
        case LVT_OrthoXY: // Top
            ViewTransformOrthographic.SetLocation(Pivot + FVector::UpVector*FarDistance);
            ViewTransformOrthographic.SetRotation(FVector(0.0f, 90.0f, -90.0f));
            break;
        case LVT_OrthoXZ: // Front
            ViewTransformOrthographic.SetLocation(Pivot + FVector::ForwardVector*FarDistance);
            ViewTransformOrthographic.SetRotation(FVector(0.0f, 0.0f, 180.0f));
            break;
        case LVT_OrthoYZ: // Left
            ViewTransformOrthographic.SetLocation(Pivot + FVector::RightVector*FarDistance);
            ViewTransformOrthographic.SetRotation(FVector(0.0f, 0.0f, 270.0f));
            break;
        case LVT_Perspective:
            break;
        case LVT_OrthoNegativeXY: // Bottom
            ViewTransformOrthographic.SetLocation(Pivot + FVector::UpVector * -1.0f*FarDistance);
            ViewTransformOrthographic.SetRotation(FVector(0.0f, -90.0f, 90.0f));
            break;
        case LVT_OrthoNegativeXZ: // Back
            ViewTransformOrthographic.SetLocation(Pivot + FVector::ForwardVector * -1.0f*FarDistance);
            ViewTransformOrthographic.SetRotation(FVector(0.0f, 0.0f, 0.0f));
            break;
        case LVT_OrthoNegativeYZ: // Right
            ViewTransformOrthographic.SetLocation(Pivot + FVector::RightVector * -1.0f*FarDistance);
            ViewTransformOrthographic.SetRotation(FVector(0.0f, 0.0f, 90.0f));
            break;
        case LVT_MAX:
        case LVT_None:
        default:
            break;
    }
}

void FEditorViewportClient::SetOrthoSize(const float InValue)
{
    OrthoSize = InValue;
    OrthoSize = FMath::Max(OrthoSize, 0.1f);
}

void FEditorViewportClient::LoadConfig(const TMap<FString, FString>& Config)
{
    const FString ViewportNum = std::to_string(ViewportIndex);
    CameraSpeedSetting = GetValueFromConfig(Config, "CameraSpeedSetting" + ViewportNum, 1);
    CameraSpeedScalar = GetValueFromConfig(Config, "CameraSpeedScalar" + ViewportNum, 1.0f);
    GridSize = GetValueFromConfig(Config, "GridSize"+ ViewportNum, 10.0f);
    ViewTransformPerspective.ViewLocation.X = GetValueFromConfig(Config, "PerspectiveCameraLocX" + ViewportNum, 0.0f);
    ViewTransformPerspective.ViewLocation.Y = GetValueFromConfig(Config, "PerspectiveCameraLocY" + ViewportNum, 0.0f);
    ViewTransformPerspective.ViewLocation.Z = GetValueFromConfig(Config, "PerspectiveCameraLocZ" + ViewportNum, 0.0f);
    ViewTransformPerspective.ViewRotation.X = GetValueFromConfig(Config, "PerspectiveCameraRotX" + ViewportNum, 0.0f);
    ViewTransformPerspective.ViewRotation.Y = GetValueFromConfig(Config, "PerspectiveCameraRotY" + ViewportNum, 0.0f);
    ViewTransformPerspective.ViewRotation.Z = GetValueFromConfig(Config, "PerspectiveCameraRotZ" + ViewportNum, 0.0f);
    ShowFlag = GetValueFromConfig(Config, "ShowFlag" + ViewportNum, 31.0f);
    ViewMode = static_cast<EViewModeIndex>(GetValueFromConfig(Config, "ViewMode" + ViewportNum, 0));
    ViewportType = static_cast<ELevelViewportType>(GetValueFromConfig(Config, "ViewportType" + ViewportNum, 3));
}
void FEditorViewportClient::SaveConfig(TMap<FString, FString>& Config) const
{
    const FString ViewportNum = std::to_string(ViewportIndex);
    Config["CameraSpeedSetting"+ ViewportNum] = std::to_string(CameraSpeedSetting);
    Config["CameraSpeedScalar"+ ViewportNum] = std::to_string(CameraSpeedScalar);
    Config["GridSize"+ ViewportNum] = std::to_string(GridSize);
    Config["PerspectiveCameraLocX" + ViewportNum] = std::to_string(ViewTransformPerspective.GetLocation().X);
    Config["PerspectiveCameraLocY" + ViewportNum] = std::to_string(ViewTransformPerspective.GetLocation().Y);
    Config["PerspectiveCameraLocZ" + ViewportNum] = std::to_string(ViewTransformPerspective.GetLocation().Z);
    Config["PerspectiveCameraRotX" + ViewportNum] = std::to_string(ViewTransformPerspective.GetRotation().X);
    Config["PerspectiveCameraRotY" + ViewportNum] = std::to_string(ViewTransformPerspective.GetRotation().Y);
    Config["PerspectiveCameraRotZ" + ViewportNum] = std::to_string(ViewTransformPerspective.GetRotation().Z);
    Config["ShowFlag"+ ViewportNum] = std::to_string(ShowFlag);
    Config["ViewMode" + ViewportNum] = std::to_string(ViewMode);
    Config["ViewportType" + ViewportNum] = std::to_string(ViewportType);
}
TMap<FString, FString> FEditorViewportClient::ReadIniFile(const FString& FilePath) const
{
    TMap<FString, FString> Config;
    std::ifstream File(*FilePath);
    std::string Line;

    while (std::getline(File, Line)) {
        if (Line.empty() || Line[0] == '[' || Line[0] == ';') continue;
        std::istringstream iss(Line);
        std::string Key, Value;
        if (std::getline(iss, Key, '=') && std::getline(iss, Value))
        {
            Config[Key] = Value;
        }
    }
    return Config;
}

void FEditorViewportClient::WriteIniFile(const FString& FilePath, const TMap<FString, FString>& Config) const
{
    std::ofstream File(*FilePath);
    for (const auto& Pair : Config)
    {
        File << *Pair.Key << "=" << *Pair.Value << "\n";
    }
}

void FEditorViewportClient::SetCameraSpeed(const float Value)
{
    CameraSpeedScalar = FMath::Clamp(Value, 0.1f, 200.0f);
}


FVector FViewportCameraTransform::GetForwardVector() const
{
    FVector Forward = FVector::ForwardVector;
    Forward = JungleMath::FVectorRotate(Forward, ViewRotation);
    return Forward;
}
FVector FViewportCameraTransform::GetRightVector() const
{
    FVector Right = FVector::RightVector;
	Right = JungleMath::FVectorRotate(Right, ViewRotation);
	return Right;
}

FVector FViewportCameraTransform::GetUpVector() const
{
    FVector Up = FVector::UpVector;
    Up = JungleMath::FVectorRotate(Up, ViewRotation);
    return Up;
}

FViewportCameraTransform::FViewportCameraTransform()
    : OrthoZoom(0)
{
}
