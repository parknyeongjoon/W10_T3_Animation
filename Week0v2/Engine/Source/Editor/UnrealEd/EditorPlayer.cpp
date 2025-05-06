#include "EditorPlayer.h"

#include "EditorViewportClient.h"
#include "ImGUI/imgui.h"

#include "LaunchEngineLoop.h"
#include "ShowFlags.h"
#include "UnrealEd.h"
#include "WindowsCursor.h"
#include "BaseGizmos/GizmoBaseComponent.h"
#include "BaseGizmos/TransformGizmo.h"
#include "Components/LightComponents/LightComponentBase.h"
#include "Components/PrimitiveComponents/MeshComponents/StaticMeshComponents/StaticMeshComponent.h"

#include "Engine/FEditorStateManager.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "LevelEditor/SLevelEditor.h"
#include "PropertyEditor/PrimitiveDrawEditor.h"
#include "UObject/UObjectIterator.h"

using namespace DirectX;

void UEditorPlayer::Initialize()
{
    FSlateAppMessageHandler* Handler = GEngineLoop.GetAppMessageHandler();
    Handler->OnMouseDownDelegate.AddLambda([this](const FPointerEvent& InMouseEvent, HWND AppWnd)
    {
        if (GEngine->GetWorld()->WorldType != EWorldType::Editor)
        {
            return;
        }
        
        if (ImGui::GetIO().WantCaptureMouse) return;
    
        if (InMouseEvent.GetEffectingButton() != EKeys::LeftMouseButton)
        {
            return;
        }

        // TODO - 해당 윈도우에만 작동하게 해야됨.
        GetCursorPos(&LastMousePosision);
        if (bLAltDown && bLCtrlDown)
        {
            MultiSelectingStart();
        }

        if (UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine))
        {
            FVector2D ClientPos = FWindowsCursor::GetClientPosition(AppWnd);
            FVector PickPosition;
            std::shared_ptr<FEditorViewportClient> ActiveViewport = EditorEngine->GetLevelEditor()->GetActiveViewportClient();
            ScreenToViewSpace(ClientPos.X, ClientPos.Y, ActiveViewport->GetViewMatrix(), ActiveViewport->GetProjectionMatrix(), PickPosition);
            bool Result = PickGizmo(PickPosition);
            if (Result == false)
            {
                PickActor(PickPosition);
            }
        }
    });
    
    Handler->OnMouseMoveDelegate.AddLambda([this](const FPointerEvent& InMouseEvent, HWND AppWnd)
    {
        if (GEngine->GetWorld()->WorldType != EWorldType::Editor)
        {
            return;
        }
        
        if (ImGui::GetIO().WantCaptureMouse)
        {
            return;
        }

        if (bMultiSeleting)
        {
            MakeMulitRect();
        }
        PickedObjControl();
    });
    
    Handler->OnMouseUpDelegate.AddLambda([this](const FPointerEvent& InMouseEvent, HWND AppWnd)
    {
        if (GEngine->GetWorld()->WorldType != EWorldType::Editor)
        {
            return;
        }
        
        if (InMouseEvent.GetEffectingButton() != EKeys::LeftMouseButton)
        {
            return;
        }
        
        bAlreadyDup = false;
        if (bMultiSeleting)
        {
            MultiSelectingEnd();
        }
        else
        {
            GetWorld()->SetPickingGizmo(nullptr);
        }
    });

    Handler->OnKeyDownDelegate.AddLambda([this](const FKeyEvent& InKeyEvent, HWND AppWnd)
    {
        if (GetKeyState(VK_RBUTTON) & 0x8000)
        {
            return;
        }
        
        if (InKeyEvent.GetInputEvent() != IE_Pressed)
        {
            return;
        }
        
        UEditorPlayer* EditorPlayer = CastChecked<UEditorEngine>(GEngine)->GetEditorPlayer();
        switch (InKeyEvent.GetCharacter())
        {
        case 'Q':
            {
                //GetWorld()->SetPickingObj(nullptr);
                break;
            }
        case 'W':
            {
                EditorPlayer->SetMode(CM_TRANSLATION);
                break;
            }
        case 'E':
            {
                EditorPlayer->SetMode(CM_ROTATION);
                break;
            }
        case 'R':
            {
                EditorPlayer->SetMode(CM_SCALE);
                break;
            }
        default:
            break;
        }
    });
}

void UEditorPlayer::Tick()
{
    Input();
}

void UEditorPlayer::MultiSelectingStart()
{
    GetCursorPos(&multiSelectingStartPos);
    bMultiSeleting = true;
}

void UEditorPlayer::MultiSelectingEnd()
{
    POINT multiSelectingEndPos;
    GetCursorPos(&multiSelectingEndPos);
    
    long leftTopX = std::min(multiSelectingStartPos.x, multiSelectingEndPos.x);
    long leftTopY = std::min(multiSelectingStartPos.y, multiSelectingEndPos.y);
    long rightBottomX = std::max(multiSelectingStartPos.x, multiSelectingEndPos.x);
    long rightBottomY =  std::max(multiSelectingStartPos.y, multiSelectingEndPos.y);

    GEngine->GetWorld()->ClearSelectedActors();

    bMultiSeleting = false;
}

void UEditorPlayer::MakeMulitRect()
{
    UE_LOG(LogLevel::Error, "MakeRecting");
    POINT multiSelectingEndPos;
    GetCursorPos(&multiSelectingEndPos);
    ImVec2 topLeft(std::min(multiSelectingStartPos.x, multiSelectingEndPos.x), std::min(multiSelectingStartPos.y, multiSelectingEndPos.y));
    ImVec2 bottomRight(std::max(multiSelectingStartPos.x, multiSelectingEndPos.x),std::max(multiSelectingStartPos.y, multiSelectingEndPos.y));
    ImU32 rectColor = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    float thickness = 2.0f;
    // ImGui::GetForegroundDrawList()->AddRect(topLeft, bottomRight, rectColor, 0.0f, 0, thickness);

    if (UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine))
    {
        static_cast<PrimitiveDrawEditor*>(EditorEngine->GetUnrealEditor()->GetEditorPanel("PrimitiveDrawEditor").get())->DrawRectInfo.Add(
            FDrawRectInfo(topLeft,bottomRight,rectColor,0.0f,0,thickness));
    }
}

void UEditorPlayer::Input()
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) return;
    if (GetAsyncKeyState(VK_LSHIFT) & 0x8000)
    {
        if (!bLShiftDown)
        {
            bLShiftDown = true;
        }
    }
    else
    {
        bLShiftDown = false;
    }
    if (GetAsyncKeyState(VK_LCONTROL) & 0x8000)
    {
        if (!bLCtrlDown)
        {
            bLCtrlDown = true;
        }
    }
    else
    {
        bLCtrlDown = false;
    }
    if (GetAsyncKeyState(VK_LMENU) & 0x8000)
    {
        if (!bLAltDown)
        {
            bLAltDown = true;
        }
    }
    else
    {
        bLAltDown =false;
    }
    
}

bool UEditorPlayer::PickGizmo(FVector& pickPosition)
{
    bool isPickedGizmo = false;
    if (!GetWorld()->GetSelectedActors().IsEmpty())
    {
        if (cMode == CM_TRANSLATION)
        {
            for (auto Iter : GetWorld()->LocalGizmo->GetArrowArr())
            {
                int maxIntersect = 0;
                float minDistance = FLT_MAX;
                float Distance = 0.0f;
                int currentIntersectCount = 0;
                if (!Iter) continue;
                if (RayIntersectsObject(pickPosition, Iter, Distance, currentIntersectCount))
                {
                    if (Distance < minDistance)
                    {
                        minDistance = Distance;
                        maxIntersect = currentIntersectCount;
                        GetWorld()->SetPickingGizmo(Iter);
                        isPickedGizmo = true;
                    }
                    else if (abs(Distance - minDistance) < FLT_EPSILON && currentIntersectCount > maxIntersect)
                    {
                        maxIntersect = currentIntersectCount;
                        GetWorld()->SetPickingGizmo(Iter);
                        isPickedGizmo = true;
                    }
                }
            }
        }
        else if (cMode == CM_ROTATION)
        {
            for (auto iter : GetWorld()->LocalGizmo->GetDiscArr())
            {
                int maxIntersect = 0;
                float minDistance = FLT_MAX;
                float Distance = 0.0f;
                int currentIntersectCount = 0;
                //UPrimitiveComponent* localGizmo = dynamic_cast<UPrimitiveComponent*>(GetWorld()->LocalGizmo[i]);
                if (!iter) continue;
                if (RayIntersectsObject(pickPosition, iter, Distance, currentIntersectCount))
                {
                    if (Distance < minDistance)
                    {
                        minDistance = Distance;
                        maxIntersect = currentIntersectCount;
                        GetWorld()->SetPickingGizmo(iter);
                        isPickedGizmo = true;
                    }
                    else if (abs(Distance - minDistance) < FLT_EPSILON && currentIntersectCount > maxIntersect)
                    {
                        maxIntersect = currentIntersectCount;
                        GetWorld()->SetPickingGizmo(iter);
                        isPickedGizmo = true;
                    }
                }
            }
        }
        else if (cMode == CM_SCALE)
        {
            for (auto iter : GetWorld()->LocalGizmo->GetScaleArr())
            {
                int maxIntersect = 0;
                float minDistance = FLT_MAX;
                float Distance = 0.0f;
                int currentIntersectCount = 0;
                if (!iter) continue;
                if (RayIntersectsObject(pickPosition, iter, Distance, currentIntersectCount))
                {
                    if (Distance < minDistance)
                    {
                        minDistance = Distance;
                        maxIntersect = currentIntersectCount;
                        GetWorld()->SetPickingGizmo(iter);
                        isPickedGizmo = true;
                    }
                    else if (abs(Distance - minDistance) < FLT_EPSILON && currentIntersectCount > maxIntersect)
                    {
                        maxIntersect = currentIntersectCount;
                        GetWorld()->SetPickingGizmo(iter);
                        isPickedGizmo = true;
                    }
                }
            }
        }
    }
    return isPickedGizmo;
}

void UEditorPlayer::PickActor(const FVector& pickPosition)
{
    UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
    if (EditorEngine == nullptr)
    {
        return;    
    }
    
    auto ActiveViewport = EditorEngine->GetLevelEditor()->GetActiveViewportClient();
    
    if (!(ActiveViewport->ShowFlag & EEngineShowFlags::SF_Primitives))
    {
        return;
    }
    
    if (!bLShiftDown)
    {
        GetWorld()->ClearSelectedActors();
    }
    
    const UActorComponent* Possible = nullptr;
    int maxIntersect = 0;
    float minDistance = FLT_MAX;
    for (const auto iter : TObjectRange<UPrimitiveComponent>())
    {
        UPrimitiveComponent* pObj;
        if (iter->IsA<UPrimitiveComponent>() || iter->IsA<ULightComponentBase>())
        {
            pObj = static_cast<UPrimitiveComponent*>(iter);
        }
        else
        {
            continue;
        }

        if (pObj && !pObj->IsA<UGizmoBaseComponent>())
        {
            float Distance = 0.0f;
            int currentIntersectCount = 0;
            if (RayIntersectsObject(pickPosition, pObj, Distance, currentIntersectCount))
            {
                if (Distance < minDistance)
                {
                    minDistance = Distance;
                    maxIntersect = currentIntersectCount;
                    Possible = pObj;
                }
                else if (abs(Distance - minDistance) < FLT_EPSILON && currentIntersectCount > maxIntersect)
                {
                    maxIntersect = currentIntersectCount;
                    Possible = pObj;
                }
            }
        }
    }
    if (Possible)
    {
        GetWorld()->AddSelectedActor(Possible->GetOwner());
    }
}

void UEditorPlayer::AddControlMode()
{
    cMode = static_cast<ControlMode>((cMode + 1) % CM_END);
}

void UEditorPlayer::AddCoordiMode()
{
    cdMode = static_cast<CoordiMode>((cdMode + 1) % CDM_END);
}

void UEditorPlayer::ScreenToViewSpace(int screenX, int screenY, const FMatrix& viewMatrix, const FMatrix& projectionMatrix, FVector& pickPosition)
{
    UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
    if (EditorEngine == nullptr)
    {
        return;    
    }
    
    auto ActiveViewport = EditorEngine->GetLevelEditor()->GetActiveViewportClient();

    
    D3D11_VIEWPORT viewport = ActiveViewport->GetD3DViewport();
    
    float viewportX = screenX - viewport.TopLeftX;
    float viewportY = screenY - viewport.TopLeftY;

    pickPosition.X = ((2.0f * viewportX / viewport.Width) - 1) / projectionMatrix[0][0];
    pickPosition.Y = -((2.0f * viewportY / viewport.Height) - 1) / projectionMatrix[1][1];
    if (ActiveViewport->IsOrtho())
    {
        pickPosition.Z = 0.0f;  // 오쏘 모드에서는 unproject 시 near plane 위치를 기준
    }
    else
    {
        pickPosition.Z = 1.0f;  // 퍼스펙티브 모드: near plane
    }
}

int UEditorPlayer::RayIntersectsObject(const FVector& pickPosition, USceneComponent* obj, float& hitDistance, int& intersectCount)
{
    UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
    if (EditorEngine == nullptr)
    {
        return -1;    
    }
    
    auto ActiveViewport = EditorEngine->GetLevelEditor()->GetActiveViewportClient();

    
	FMatrix scaleMatrix = FMatrix::CreateScaleMatrix(
		obj->GetWorldScale().X,
		obj->GetWorldScale().Y,
		obj->GetWorldScale().Z
	);
	FMatrix rotationMatrix = FMatrix::CreateRotationMatrix(
		obj->GetWorldRotation().Roll,
		obj->GetWorldRotation().Pitch,
		obj->GetWorldRotation().Yaw
	);

	FMatrix translationMatrix = FMatrix::CreateTranslationMatrix(obj->GetWorldLocation());

	// ���� ��ȯ ���
	FMatrix worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;
	FMatrix viewMatrix = ActiveViewport->GetViewMatrix();
    
    bool bIsOrtho = ActiveViewport->IsOrtho();
    

    if (bIsOrtho)
    {
        // 오쏘 모드: ScreenToViewSpace()에서 계산된 pickPosition이 클립/뷰 좌표라고 가정
        FMatrix inverseView = FMatrix::Inverse(viewMatrix);
        // pickPosition을 월드 좌표로 변환
        FVector worldPickPos = inverseView.TransformPosition(pickPosition);  
        // 오쏘에서는 픽킹 원점은 unproject된 픽셀의 위치
        FVector rayOrigin = worldPickPos;
        // 레이 방향은 카메라의 정면 방향 (평행)
        FVector orthoRayDir = ActiveViewport->ViewTransformOrthographic.GetForwardVector().Normalize();

        // 객체의 로컬 좌표계로 변환
        FMatrix localMatrix = FMatrix::Inverse(worldMatrix);
        FVector localRayOrigin = localMatrix.TransformPosition(rayOrigin);
        FVector localRayDir = (localMatrix.TransformPosition(rayOrigin + orthoRayDir) - localRayOrigin).Normalize();
        
        intersectCount = obj->CheckRayIntersection(localRayOrigin, localRayDir, hitDistance);
        return intersectCount;
    }
    else
    {
        FMatrix inverseMatrix = FMatrix::Inverse(worldMatrix * viewMatrix);
        FVector cameraOrigin = { 0,0,0 };
        FVector pickRayOrigin = inverseMatrix.TransformPosition(cameraOrigin);
        // 퍼스펙티브 모드의 기존 로직 사용
        FVector transformedPick = inverseMatrix.TransformPosition(pickPosition);
        FVector rayDirection = (transformedPick - pickRayOrigin).Normalize();
        
        intersectCount = obj->CheckRayIntersection(pickRayOrigin, rayDirection, hitDistance);
        return intersectCount;
    }
}

void UEditorPlayer::PickedObjControl()
{
    
    if (GetWorld()->GetPickingGizmo())
    {
        POINT currentMousePos;
        GetCursorPos(&currentMousePos);
        int32 deltaX = currentMousePos.x - LastMousePosision.x;
        int32 deltaY = currentMousePos.y - LastMousePosision.y;

        // USceneComponent* pObj = GetWorld()->GetPickingObj();
        //AActor* PickedActor = GetWorld()->GetSelectedActors();
        for (auto pickedActor : GetWorld()->GetSelectedActors())
        {
            if (pickedActor== nullptr)
                continue;
            UGizmoBaseComponent* Gizmo = static_cast<UGizmoBaseComponent*>(GetWorld()->GetPickingGizmo());
            switch (cMode)
            {
            case CM_TRANSLATION:
                // SLevelEditor에 있음.
                ControlTranslation(pickedActor->GetRootComponent(), Gizmo, deltaX, deltaY);
                break;
            case CM_SCALE:
                ControlScale(pickedActor->GetRootComponent(), Gizmo, deltaX, deltaY);
                break;
            case CM_ROTATION:
                ControlRotation(pickedActor->GetRootComponent(), Gizmo, deltaX, deltaY);
                break;
            default:
                break;
            }
        }
        LastMousePosision = currentMousePos;
    }
        
}

void UEditorPlayer::ControlRotation(USceneComponent* pObj, UGizmoBaseComponent* Gizmo, int32 deltaX, int32 deltaY)
{
    UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
    if (EditorEngine == nullptr)
    {
        return;    
    }
    
    auto ActiveViewport = EditorEngine->GetLevelEditor()->GetActiveViewportClient();
    
    FVector cameraForward = ActiveViewport->ViewTransformPerspective.GetForwardVector();
    FVector cameraRight = ActiveViewport->ViewTransformPerspective.GetRightVector();
    FVector cameraUp = ActiveViewport->ViewTransformPerspective.GetUpVector();

    FQuat currentRotation = pObj->GetWorldRotation().ToQuaternion();

    FQuat rotationDelta;

    if (bLAltDown && !bAlreadyDup)
    {
        GetWorld()->DuplicateSelectedActorsOnLocation();
        bAlreadyDup = true;
    }
    if (Gizmo->GetGizmoType() == UGizmoBaseComponent::CircleX)
    {
        float rotationAmount = (cameraUp.Z >= 0 ? -1.0f : 1.0f) * deltaY * 0.01f;
        rotationAmount = rotationAmount + (cameraRight.X >= 0 ? 1.0f : -1.0f) * deltaX * 0.01f;

        rotationDelta = FQuat(FVector(1.0f, 0.0f, 0.0f), rotationAmount); // ���� X �� ���� ȸ��
    }
    else if (Gizmo->GetGizmoType() == UGizmoBaseComponent::CircleY)
    {
        float rotationAmount = (cameraRight.X >= 0 ? 1.0f : -1.0f) * deltaX * 0.01f;
        rotationAmount = rotationAmount + (cameraUp.Z >= 0 ? 1.0f : -1.0f) * deltaY * 0.01f;

        rotationDelta = FQuat(FVector(0.0f, 1.0f, 0.0f), rotationAmount); // ���� Y �� ���� ȸ��
    }
    else if (Gizmo->GetGizmoType() == UGizmoBaseComponent::CircleZ)
    {
        float rotationAmount = (cameraForward.X <= 0 ? -1.0f : 1.0f) * deltaX * 0.01f;
        rotationDelta = FQuat(FVector(0.0f, 0.0f, 1.0f), rotationAmount); // ���� Z �� ���� ȸ��
    }
    if (cdMode == CDM_LOCAL)
    {
        pObj->SetRelativeRotation(currentRotation * rotationDelta);
    }
    else if (cdMode == CDM_WORLD)
    {
        pObj->SetRelativeRotation(rotationDelta * currentRotation);
    }
}

void UEditorPlayer::ControlTranslation(USceneComponent* pObj, UGizmoBaseComponent* Gizmo, int32 deltaX, int32 deltaY)
{
    float DeltaX = static_cast<float>(deltaX);
    float DeltaY = static_cast<float>(deltaY);

    UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
    if (EditorEngine == nullptr)
    {
        return;    
    }
    
    auto ActiveViewport = EditorEngine->GetLevelEditor()->GetActiveViewportClient();

    FVector CamearRight = ActiveViewport->GetViewportType() == LVT_Perspective ? ActiveViewport->ViewTransformPerspective.GetRightVector() : ActiveViewport->ViewTransformOrthographic.GetRightVector();
    FVector CameraUp = ActiveViewport->GetViewportType() == LVT_Perspective ? ActiveViewport->ViewTransformPerspective.GetUpVector() : ActiveViewport->ViewTransformOrthographic.GetUpVector();
    
    FVector WorldMoveDirection = (CamearRight * DeltaX + CameraUp * -DeltaY) * 0.1f;

    if (bLAltDown && !bAlreadyDup)
    {
        GetWorld()->DuplicateSelectedActorsOnLocation();
        bAlreadyDup = true;
    }
    if (cdMode == CDM_LOCAL)
    {
        if (Gizmo->GetGizmoType() == UGizmoBaseComponent::ArrowX)
        {
            float moveAmount = WorldMoveDirection.Dot(pObj->GetWorldForwardVector());
            pObj->AddWorldLocation(pObj->GetWorldForwardVector() * moveAmount);
        }
        else if (Gizmo->GetGizmoType() == UGizmoBaseComponent::ArrowY)
        {
            float moveAmount = WorldMoveDirection.Dot(pObj->GetWorldRightVector());
            pObj->AddWorldLocation(pObj->GetWorldRightVector() * moveAmount);
        }
        else if (Gizmo->GetGizmoType() == UGizmoBaseComponent::ArrowZ)
        {
            float moveAmount = WorldMoveDirection.Dot(pObj->GetWorldUpVector());
            pObj->AddWorldLocation(pObj->GetWorldUpVector() * moveAmount);
        }
    }
    else if (cdMode == CDM_WORLD)
    {
        float Distance = (ActiveViewport->ViewTransformPerspective.GetLocation() - (*GEngine->GetWorld()->GetSelectedActors().begin())->GetActorLocation()).Magnitude();
        Distance /= 100.0f;
        // 월드 좌표계에서 카메라 방향을 고려한 이동
        if (Gizmo->GetGizmoType() == UGizmoBaseComponent::ArrowX)
        {
            // 카메라의 오른쪽 방향을 X축 이동에 사용
            FVector moveDir = CamearRight * DeltaX * 0.05f;
            pObj->AddWorldLocation(FVector(moveDir.X, 0.0f, 0.0f) * Distance);
        }
        else if (Gizmo->GetGizmoType() == UGizmoBaseComponent::ArrowY)
        {
            // 카메라의 오른쪽 방향을 Y축 이동에 사용
            FVector moveDir = CamearRight * DeltaX * 0.05f;
            pObj->AddWorldLocation(FVector(0.0f, moveDir.Y, 0.0f) * Distance);
        }
        else if (Gizmo->GetGizmoType() == UGizmoBaseComponent::ArrowZ)
        {
            // 카메라의 위쪽 방향을 Z축 이동에 사용
            FVector moveDir = CameraUp * -DeltaY * 0.05f;
            pObj->AddWorldLocation(FVector(0.0f, 0.0f, moveDir.Z) * Distance);
        }
    }
}

void UEditorPlayer::ControlScale(USceneComponent* pObj, UGizmoBaseComponent* Gizmo, int32 deltaX, int32 deltaY)
{
    float DeltaX = static_cast<float>(deltaX);
    float DeltaY = static_cast<float>(deltaY);
    if (UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine))
    {
        auto ActiveViewport = EditorEngine->GetLevelEditor()->GetActiveViewportClient();

        FVector CamearRight = ActiveViewport->GetViewportType() == LVT_Perspective ? ActiveViewport->ViewTransformPerspective.GetRightVector() : ActiveViewport->ViewTransformOrthographic.GetRightVector();
        FVector CameraUp = ActiveViewport->GetViewportType() == LVT_Perspective ? ActiveViewport->ViewTransformPerspective.GetUpVector() : ActiveViewport->ViewTransformOrthographic.GetUpVector();
    
        // 월드 좌표계에서 카메라 방향을 고려한 이동
        if (Gizmo->GetGizmoType() == UGizmoBaseComponent::ScaleX)
        {
            // 카메라의 오른쪽 방향을 X축 이동에 사용
            FVector moveDir = CamearRight * DeltaX * 0.05f;
            pObj->AddRelativeScale(FVector(moveDir.X, 0.0f, 0.0f));
        }
        else if (Gizmo->GetGizmoType() == UGizmoBaseComponent::ScaleY)
        {
            // 카메라의 오른쪽 방향을 Y축 이동에 사용
            FVector moveDir = CamearRight * DeltaX * 0.05f;
            pObj->AddRelativeScale(FVector(0.0f, moveDir.Y, 0.0f));
        }
        else if (Gizmo->GetGizmoType() == UGizmoBaseComponent::ScaleZ)
        {
            // 카메라의 위쪽 방향을 Z축 이동에 사용
            FVector moveDir = CameraUp * -DeltaY * 0.05f;
            pObj->AddRelativeScale(FVector(0.0f, 0.0f, moveDir.Z));
        }
    }
}
