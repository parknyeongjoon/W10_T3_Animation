#include "EditorPlayer.h"



#include <cmath>
#include "Math/Quat.h"
#include "HAL/PlatformType.h"
#include "EngineBaseTypes.h"
#include "UObject/ObjectTypes.h"
#include "UObject/UObjectIterator.h"

#include "EditorEngine.h"
#include "Engine/FEditorStateManager.h"
#include "Viewport.h"
#include "EditorViewportClient.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

#include "ImGUI/imgui.h"
#include "SlateCore/Layout/SlateRect.h"
#include "UnrealEd.h"
#include "LevelEditor/SLevelEditor.h"
#include "PropertyEditor/PrimitiveDrawEditor.h"
#include "UserInterface/Console.h"
#include "ShowFlags.h"


#include "BaseGizmos/TransformGizmo.h"
#include "BaseGizmos/GizmoBaseComponent.h"
#include "Components/LightComponents/LightComponentBase.h"

using namespace DirectX;

void UEditorPlayer::Initialize()
{
}

void UEditorPlayer::MultiSelectingStart()
{
    GetCursorPos(&multiSelectingStartPos);
    bMultiSeleting = true;
}

void UEditorPlayer::MultiSelectingEnd(UWorld* World)
{
    POINT multiSelectingEndPos;
    GetCursorPos(&multiSelectingEndPos);
    /*
    long LeftTopX = std::min(multiSelectingStartPos.x, multiSelectingEndPos.x);
    long LeftTopY = std::min(multiSelectingStartPos.y, multiSelectingEndPos.y);
    long RightBottomX = std::max(multiSelectingStartPos.x, multiSelectingEndPos.x);
    long RightBottomY = std::max(multiSelectingStartPos.y, multiSelectingEndPos.y);
    */
    World->ClearSelectedActors();

    bMultiSeleting = false;
}

void UEditorPlayer::MakeMulitRect() const
{
    UE_LOG(LogLevel::Error, "Make Multi Rect");
    POINT MultiSelectingEndPos;
    GetCursorPos(&MultiSelectingEndPos);
    const ImVec2 TopLeft(std::min(multiSelectingStartPos.x, MultiSelectingEndPos.x), std::min(multiSelectingStartPos.y, MultiSelectingEndPos.y));
    const ImVec2 BottomRight(std::max(multiSelectingStartPos.x, MultiSelectingEndPos.x), std::max(multiSelectingStartPos.y, MultiSelectingEndPos.y));
    const ImU32 RectColor = ImGui::GetColorU32(ImVec4(1.0F, 1.0F, 1.0F, 1.0F));
    // ImGui::GetForegroundDrawList()->AddRect(topLeft, bottomRight, rectColor, 0.0f, 0, thickness);

    if (const UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine))
    {
        constexpr float Thickness = 2.0F;
        static_cast<PrimitiveDrawEditor*>(EditorEngine->GetUnrealEditor()->GetEditorPanel("PrimitiveDrawEditor").get())->DrawRectInfo.Add(
            FDrawRectInfo(TopLeft, BottomRight, RectColor, 0.0F, 0, Thickness));
    }
}


auto UEditorPlayer::PickGizmo(const ControlMode ControlMode, UWorld* World, const FVector& PickPosition) -> bool
{
    bool bIsGizmoPicked = false;

    if (!World->GetSelectedActors().IsEmpty())
    {
        int MaxIntersect = 0;
        float MinDistance = FLT_MAX;
        float Distance = 0.0f;
        int CurrentIntersectCount = 0;

        TArray<UStaticMeshComponent*>& LocalGizmo =
            ControlMode == CM_TRANSLATION ? World->LocalGizmo->GetArrowArr() :
            ControlMode == CM_ROTATION ? World->LocalGizmo->GetDiscArr() :
            World->LocalGizmo->GetScaleArr();   // ControlMode == CM_SCALE

        for (const auto iter : LocalGizmo)
        {
            if (!iter)
            {
                continue;
            }

            if (RayIntersectsObject(PickPosition, iter, Distance, CurrentIntersectCount))
            {
                if (Distance < MinDistance)
                {
                    MinDistance = Distance;
                    MaxIntersect = CurrentIntersectCount;
                    World->SetPickingGizmo(iter);
                    bIsGizmoPicked = true;
                }
                else if (abs(Distance - MinDistance) < FLT_EPSILON && CurrentIntersectCount > MaxIntersect)
                {
                    MaxIntersect = CurrentIntersectCount;
                    World->SetPickingGizmo(iter);
                    bIsGizmoPicked = true;
                }
            }
        }
    }

    return bIsGizmoPicked;
}

void UEditorPlayer::PickActor(UWorld* World, const FVector& PickPosition) const
{
    const UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
    if (EditorEngine == nullptr)
    {
        return;
    }

    const auto ActiveViewport = EditorEngine->GetLevelEditor()->GetActiveViewportClient();

    if (!(ActiveViewport->ShowFlag & EEngineShowFlags::SF_Primitives))
    {
        return;
    }

    if (!bLShiftDown)
    {
        World->ClearSelectedActors();
    }

    const UActorComponent* Possible = nullptr;
    int MaxIntersect = 0;
    float MinDistance = FLT_MAX;

    for (const auto iter : TObjectRange<UPrimitiveComponent>())
    {
        if (iter->GetWorld() != World)
        {
            continue;
        }

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
            int CurrentIntersectCount = 0;
            if (RayIntersectsObject(PickPosition, pObj, Distance, CurrentIntersectCount))
            {
                if (Distance < MinDistance)
                {
                    MinDistance = Distance;
                    MaxIntersect = CurrentIntersectCount;
                    Possible = pObj;
                }
                else if (abs(Distance - MinDistance) < FLT_EPSILON && CurrentIntersectCount > MaxIntersect)
                {
                    MaxIntersect = CurrentIntersectCount;
                    Possible = pObj;
                }
            }
        }
    }

    if (Possible != nullptr)
    {
        World->AddSelectedActor(Possible->GetOwner());
    }
}

void UEditorPlayer::ScreenToViewSpace(const int ScreenX, const int ScreenY, const FMatrix& /*viewMatrix*/, const FMatrix& ProjectionMatrix,
                                      FVector& RayOrigin)
{
    const UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
    if (EditorEngine == nullptr)
    {
        return;
    }

    const auto ActiveViewport = EditorEngine->GetLevelEditor()->GetActiveViewportClient();


    const FRect ViewportRect = ActiveViewport->GetViewport()->GetFSlateRect();

    const float ViewportX = static_cast<float>(ScreenX) - ViewportRect.LeftTopX;
    const float ViewportY = static_cast<float>(ScreenY) - ViewportRect.LeftTopY;

    RayOrigin.X = (2.0F * ViewportX / ViewportRect.Width - 1) / ProjectionMatrix[0][0];
    RayOrigin.Y = -((2.0F * ViewportY / ViewportRect.Height) - 1) / ProjectionMatrix[1][1];

    if (ActiveViewport->IsOrtho())
    {
        RayOrigin.Z = 0.0F; // 오쏘 모드에서는 unproject 시 near plane 위치를 기준
    }
    else
    {
        RayOrigin.Z = 1.0F; // 퍼스펙티브 모드: near plane
    }
}

auto UEditorPlayer::RayIntersectsObject(const FVector& PickPosition, USceneComponent* Component, float& HitDistance, int& IntersectCount) -> int
{
    const UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
    if (EditorEngine == nullptr)
    {
        return -1;
    }

    const auto ActiveViewport = EditorEngine->GetLevelEditor()->GetActiveViewportClient();

    const FMatrix WorldMatrix = Component->GetWorldMatrix();
    const FMatrix ViewMatrix = ActiveViewport->GetViewMatrix();


    if (ActiveViewport->IsOrtho())
    {
        // 오쏘 모드: ScreenToViewSpace()에서 계산된 pickPosition이 클립/뷰 좌표라고 가정
        const FMatrix InverseView = FMatrix::Inverse(ViewMatrix);
        // pickPosition을 월드 좌표로 변환
        const FVector WorldPickPos = InverseView.TransformPosition(PickPosition);
        // 오쏘에서는 픽킹 원점은 unproject된 픽셀의 위치
        const FVector RayOrigin = WorldPickPos;
        // 레이 방향은 카메라의 정면 방향 (평행)
        const FVector OrthoRayDir = ActiveViewport->ViewTransformOrthographic.GetForwardVector().GetSafeNormal();

        // 객체의 로컬 좌표계로 변환
        const FMatrix LocalMatrix = FMatrix::Inverse(WorldMatrix);
        FVector LocalRayOrigin = LocalMatrix.TransformPosition(RayOrigin);
        FVector LocalRayDir = (LocalMatrix.TransformPosition(RayOrigin + OrthoRayDir) - LocalRayOrigin).GetSafeNormal();

        IntersectCount = Component->CheckRayIntersection(LocalRayOrigin, LocalRayDir, HitDistance);

        return IntersectCount;
    }

    const FMatrix InverseMatrix = FMatrix::Inverse(WorldMatrix * ViewMatrix);
    const FVector CameraOrigin = {0, 0, 0};
    FVector PickRayOrigin = InverseMatrix.TransformPosition(CameraOrigin);
    // 퍼스펙티브 모드의 기존 로직 사용
    const FVector TransformedPick = InverseMatrix.TransformPosition(PickPosition);
    FVector RayDirection = (TransformedPick - PickRayOrigin).GetSafeNormal();

    IntersectCount = Component->CheckRayIntersection(PickRayOrigin, RayDirection, HitDistance);

    if (IntersectCount > 0)
    {
        const FVector LocalHitPoint = PickRayOrigin + RayDirection * HitDistance;

        const FVector WorldHitPoint = WorldMatrix.TransformPosition(LocalHitPoint);

        const FMatrix InverseView = FMatrix::Inverse(ViewMatrix);
        const FVector WorldRayOrigin = InverseView.TransformPosition(CameraOrigin);

        const float WorldDistance = FVector::Distance(WorldRayOrigin, WorldHitPoint);

        HitDistance = WorldDistance;
    }

    return IntersectCount;
}

void UEditorPlayer::PickedObjControl(const ControlMode ControlMode, const CoordiMode CoordiMode, UWorld* World)
{
    if (World->GetPickingGizmo() != nullptr)
    {
        POINT CurrentMousePos;
        GetCursorPos(&CurrentMousePos);
        const int32 DeltaX = CurrentMousePos.x - LastMousePosision.x;
        const int32 DeltaY = CurrentMousePos.y - LastMousePosision.y;

        // USceneComponent* pObj = World->GetPickingObj();
        //AActor* PickedActor = World->GetSelectedActors();
        for (const auto PickedActor : World->GetSelectedActors())
        {
            if (PickedActor == nullptr)
            {
                continue;
            }

            const UGizmoBaseComponent* Gizmo = static_cast<UGizmoBaseComponent*>(World->GetPickingGizmo());

            switch (ControlMode)
            {
            case CM_TRANSLATION:
                // SLevelEditor에 있음.
                ControlTranslation(CoordiMode, World, PickedActor->GetRootComponent(), Gizmo, DeltaX, DeltaY);
                break;
            case CM_SCALE:
                ControlScale(PickedActor->GetRootComponent(), Gizmo, DeltaX, DeltaY);
                break;
            case CM_ROTATION:
                ControlRotation(CoordiMode, World, PickedActor->GetRootComponent(), Gizmo, DeltaX, DeltaY);
                break;
            default:
                break;
            }
        }
        LastMousePosision = CurrentMousePos;
    }
}

void UEditorPlayer::ControlRotation(const CoordiMode CoordiMode, UWorld* World, USceneComponent* pObj, const UGizmoBaseComponent* Gizmo,
                                    const int32 DeltaX, const int32 DeltaY)
{
    const auto FDeltaX = static_cast<float>(DeltaX);
    const auto FDeltaY = static_cast<float>(DeltaY);

    const UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
    if (EditorEngine == nullptr)
    {
        return;
    }

    const auto ActiveViewport = EditorEngine->GetLevelEditor()->GetActiveViewportClient();

    const FVector CameraForward = ActiveViewport->ViewTransformPerspective.GetForwardVector();
    const FVector CameraRight = ActiveViewport->ViewTransformPerspective.GetRightVector();
    const FVector CameraUp = ActiveViewport->ViewTransformPerspective.GetUpVector();

    const FQuat CurrentRotation = pObj->GetWorldRotation().ToQuaternion();


    if (bLAltDown && !bAlreadyDup)
    {
        World->DuplicateSelectedActorsOnLocation();
        bAlreadyDup = true;
    }

    FQuat RotationDelta;
    if (Gizmo->GetGizmoType() == UGizmoBaseComponent::CircleX)
    {
        float RotationAmount = (CameraUp.Z >= 0 ? -1.0F : 1.0F) * FDeltaY * 0.01F;
        RotationAmount = RotationAmount + (CameraRight.X >= 0 ? 1.0F : -1.0F) * FDeltaX * 0.01F;

        RotationDelta = FQuat(FVector(1.0F, 0.0F, 0.0F), RotationAmount);
    }
    else if (Gizmo->GetGizmoType() == UGizmoBaseComponent::CircleY)
    {
        float RotationAmount = (CameraRight.X >= 0 ? 1.0F : -1.0F) * FDeltaX * 0.01F;
        RotationAmount = RotationAmount + (CameraUp.Z >= 0 ? 1.0F : -1.0F) * FDeltaY * 0.01F;

        RotationDelta = FQuat(FVector(0.0F, 1.0F, 0.0F), RotationAmount);
    }
    else if (Gizmo->GetGizmoType() == UGizmoBaseComponent::CircleZ)
    {
        const float RotationAmount = (CameraForward.X <= 0 ? -1.0F : 1.0F) * FDeltaX * 0.01F;
        RotationDelta = FQuat(FVector(0.0F, 0.0F, 1.0F), RotationAmount);
    }
    if (CoordiMode == CDM_LOCAL)
    {
        pObj->SetRelativeRotation(CurrentRotation * RotationDelta);
    }
    else if (CoordiMode == CDM_WORLD)
    {
        pObj->SetRelativeRotation(RotationDelta * CurrentRotation);
    }
}

void UEditorPlayer::ControlTranslation(const CoordiMode CoordiMode, UWorld* World, USceneComponent* pObj, const UGizmoBaseComponent* Gizmo,
                                       const int32 DeltaX, const int32 DeltaY)
{
    const auto FDeltaX = static_cast<float>(DeltaX);
    const auto FDeltaY = static_cast<float>(DeltaY);

    const UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
    if (EditorEngine == nullptr)
    {
        return;
    }

    const auto ActiveViewport = EditorEngine->GetLevelEditor()->GetActiveViewportClient();

    const FVector CamearRight = ActiveViewport->GetViewportType() == LVT_Perspective
                                    ? ActiveViewport->ViewTransformPerspective.GetRightVector()
                                    : ActiveViewport->ViewTransformOrthographic.GetRightVector();
    const FVector CameraUp = ActiveViewport->GetViewportType() == LVT_Perspective
                                 ? ActiveViewport->ViewTransformPerspective.GetUpVector()
                                 : ActiveViewport->ViewTransformOrthographic.GetUpVector();

    const FVector WorldMoveDirection = (CamearRight * FDeltaX + CameraUp * -FDeltaY) * 0.1F;

    if (bLAltDown && !bAlreadyDup)
    {
        World->DuplicateSelectedActorsOnLocation();
        bAlreadyDup = true;
    }

    if (CoordiMode == CDM_LOCAL)
    {
        if (Gizmo->GetGizmoType() == UGizmoBaseComponent::ArrowX)
        {
            const float MoveAmount = WorldMoveDirection.Dot(pObj->GetWorldForwardVector());
            pObj->AddWorldLocation(pObj->GetWorldForwardVector() * MoveAmount);
        }
        else if (Gizmo->GetGizmoType() == UGizmoBaseComponent::ArrowY)
        {
            const float MoveAmount = WorldMoveDirection.Dot(pObj->GetWorldRightVector());
            pObj->AddWorldLocation(pObj->GetWorldRightVector() * MoveAmount);
        }
        else if (Gizmo->GetGizmoType() == UGizmoBaseComponent::ArrowZ)
        {
            const float MoveAmount = WorldMoveDirection.Dot(pObj->GetWorldUpVector());
            pObj->AddWorldLocation(pObj->GetWorldUpVector() * MoveAmount);
        }
    }
    else if (CoordiMode == CDM_WORLD)
    {
        float Distance = (ActiveViewport->ViewTransformPerspective.GetLocation() - (*World->GetSelectedActors().begin())->GetActorLocation()).Magnitude();
        Distance /= 100.0F;

        // 월드 좌표계에서 카메라 방향을 고려한 이동
        if (Gizmo->GetGizmoType() == UGizmoBaseComponent::ArrowX)
        {
            // 카메라의 오른쪽 방향을 X축 이동에 사용
            const FVector MoveDir = CamearRight * FDeltaX * 0.05F;
            pObj->AddWorldLocation(FVector(MoveDir.X, 0.0F, 0.0F) * Distance);
        }
        else if (Gizmo->GetGizmoType() == UGizmoBaseComponent::ArrowY)
        {
            // 카메라의 오른쪽 방향을 Y축 이동에 사용
            const FVector MoveDir = CamearRight * FDeltaX * 0.05F;
            pObj->AddWorldLocation(FVector(0.0F, MoveDir.Y, 0.0F) * Distance);
        }
        else if (Gizmo->GetGizmoType() == UGizmoBaseComponent::ArrowZ)
        {
            // 카메라의 위쪽 방향을 Z축 이동에 사용
            const FVector MoveDir = CameraUp * -FDeltaY * 0.05F;
            pObj->AddWorldLocation(FVector(0.0F, 0.0F, MoveDir.Z) * Distance);
        }
    }
}

void UEditorPlayer::ControlScale(USceneComponent* pObj, const UGizmoBaseComponent* Gizmo, const int32 DeltaX, const int32 DeltaY)
{
    const auto FDeltaX = static_cast<float>(DeltaX);
    const auto FDeltaY = static_cast<float>(DeltaY);

    if (const UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine))
    {
        const auto ActiveViewport = EditorEngine->GetLevelEditor()->GetActiveViewportClient();

        const FVector CamearRight = ActiveViewport->GetViewportType() == LVT_Perspective
                                        ? ActiveViewport->ViewTransformPerspective.GetRightVector()
                                        : ActiveViewport->ViewTransformOrthographic.GetRightVector();
        const FVector CameraUp = ActiveViewport->GetViewportType() == LVT_Perspective
                                     ? ActiveViewport->ViewTransformPerspective.GetUpVector()
                                     : ActiveViewport->ViewTransformOrthographic.GetUpVector();

        // 월드 좌표계에서 카메라 방향을 고려한 이동
        if (Gizmo->GetGizmoType() == UGizmoBaseComponent::ScaleX)
        {
            // 카메라의 오른쪽 방향을 X축 이동에 사용
            const FVector MoveDir = CamearRight * FDeltaX * 0.05F;
            pObj->AddRelativeScale(FVector(MoveDir.X, 0.0F, 0.0F));
        }
        else if (Gizmo->GetGizmoType() == UGizmoBaseComponent::ScaleY)
        {
            // 카메라의 오른쪽 방향을 Y축 이동에 사용
            const FVector MoveDir = CamearRight * FDeltaX * 0.05F;
            pObj->AddRelativeScale(FVector(0.0F, MoveDir.Y, 0.0F));
        }
        else if (Gizmo->GetGizmoType() == UGizmoBaseComponent::ScaleZ)
        {
            // 카메라의 위쪽 방향을 Z축 이동에 사용
            const FVector MoveDir = CameraUp * -FDeltaY * 0.05F;
            pObj->AddRelativeScale(FVector(0.0F, 0.0F, MoveDir.Z));
        }
    }
}
