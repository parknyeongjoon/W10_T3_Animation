#include "TransformGizmo.h"
#include "GizmoArrowComponent.h"
#include "GizmoCircleComponent.h"
#include "GizmoRectangleComponent.h"
#include "Engine/World.h"
#include "Engine/FLoaderOBJ.h"
#include "LevelEditor/SLevelEditor.h"
#include "UnrealEd/EditorPlayer.h"

UTransformGizmo::UTransformGizmo()
{
    FManagerOBJ::CreateStaticMesh("Assets/gizmo_loc_x.obj");
    FManagerOBJ::CreateStaticMesh("Assets/gizmo_loc_y.obj");
    FManagerOBJ::CreateStaticMesh("Assets/gizmo_loc_z.obj");
    FManagerOBJ::CreateStaticMesh("Assets/gizmo_rot_x.obj");
    FManagerOBJ::CreateStaticMesh("Assets/gizmo_rot_y.obj");
    FManagerOBJ::CreateStaticMesh("Assets/gizmo_rot_z.obj");
    FManagerOBJ::CreateStaticMesh("Assets/gizmo_scale_x.obj");
    FManagerOBJ::CreateStaticMesh("Assets/gizmo_scale_y.obj");
    FManagerOBJ::CreateStaticMesh("Assets/gizmo_scale_z.obj");

    SetRootComponent(
        AddComponent<USceneComponent>(EComponentOrigin::Constructor)
    );

    UGizmoArrowComponent* LocationX = AddComponent<UGizmoArrowComponent>(EComponentOrigin::Constructor);
    LocationX->SetStaticMesh(FManagerOBJ::GetStaticMesh(L"gizmo_loc_x.obj"));
	LocationX->SetupAttachment(RootComponent);
    LocationX->SetGizmoType(UGizmoBaseComponent::ArrowX);
	ArrowArr.Add(LocationX);

    UGizmoArrowComponent* LocationY = AddComponent<UGizmoArrowComponent>(EComponentOrigin::Constructor);
    LocationY->SetStaticMesh(FManagerOBJ::GetStaticMesh(L"gizmo_loc_y.obj"));
    LocationY->SetupAttachment(RootComponent);
    LocationY->SetGizmoType(UGizmoBaseComponent::ArrowY);
    ArrowArr.Add(LocationY);

    UGizmoArrowComponent* LocationZ = AddComponent<UGizmoArrowComponent>(EComponentOrigin::Constructor);
    LocationZ->SetStaticMesh(FManagerOBJ::GetStaticMesh(L"gizmo_loc_z.obj"));
    LocationZ->SetupAttachment(RootComponent);
    LocationZ->SetGizmoType(UGizmoBaseComponent::ArrowZ);
    ArrowArr.Add(LocationZ);

    UGizmoRectangleComponent* ScaleX = AddComponent<UGizmoRectangleComponent>(EComponentOrigin::Constructor);
    ScaleX->SetStaticMesh(FManagerOBJ::GetStaticMesh(L"gizmo_scale_x.obj"));
    ScaleX->SetupAttachment(RootComponent);
    ScaleX->SetGizmoType(UGizmoBaseComponent::ScaleX);
    RectangleArr.Add(ScaleX);

    UGizmoRectangleComponent* ScaleY = AddComponent<UGizmoRectangleComponent>(EComponentOrigin::Constructor);
    ScaleY->SetStaticMesh(FManagerOBJ::GetStaticMesh(L"gizmo_scale_y.obj"));
    ScaleY->SetupAttachment(RootComponent);
    ScaleY->SetGizmoType(UGizmoBaseComponent::ScaleY);
    RectangleArr.Add(ScaleY);

    UGizmoRectangleComponent* ScaleZ = AddComponent<UGizmoRectangleComponent>(EComponentOrigin::Constructor);
    ScaleZ->SetStaticMesh(FManagerOBJ::GetStaticMesh(L"gizmo_scale_z.obj"));
    ScaleZ->SetupAttachment(RootComponent);
    ScaleZ->SetGizmoType(UGizmoBaseComponent::ScaleZ);
    RectangleArr.Add(ScaleZ);

    UGizmoCircleComponent* CircleX = AddComponent<UGizmoCircleComponent>(EComponentOrigin::Constructor);
    CircleX->SetStaticMesh(FManagerOBJ::GetStaticMesh(L"gizmo_rot_x.obj"));
    CircleX->SetupAttachment(RootComponent);
    CircleX->SetGizmoType(UGizmoBaseComponent::CircleX);
    CircleArr.Add(CircleX);

    UGizmoCircleComponent* CircleY = AddComponent<UGizmoCircleComponent>(EComponentOrigin::Constructor);
    CircleY->SetStaticMesh(FManagerOBJ::GetStaticMesh(L"gizmo_rot_y.obj"));
    CircleY->SetupAttachment(RootComponent);
    CircleY->SetGizmoType(UGizmoBaseComponent::CircleY);
    CircleArr.Add(CircleY);

    UGizmoCircleComponent* CircleZ = AddComponent<UGizmoCircleComponent>(EComponentOrigin::Constructor);
    CircleZ->SetStaticMesh(FManagerOBJ::GetStaticMesh(L"gizmo_rot_z.obj"));
    CircleZ->SetupAttachment(RootComponent);
    CircleZ->SetGizmoType(UGizmoBaseComponent::CircleZ);
    CircleArr.Add(CircleZ);
}

void UTransformGizmo::Tick(const float DeltaTime)
{
    if (GetWorld()->WorldType != EWorldType::Editor
     || GetWorld()->WorldType != EWorldType::EditorPreview)
    {
        return;
    }

	Super::Tick(DeltaTime);

    TSet<AActor*> SelectedActors = GetWorld()->GetSelectedActors();

    // @todo 단일 선택에 대한 케이스 분리
    if (!SelectedActors.IsEmpty())
    {
        const AActor* PickedActor = *SelectedActors.begin();
        if (PickedActor == nullptr)
        {
            return;
        }

        SetActorLocation(PickedActor->GetActorLocation());

        if (UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine))
        {
            ControlMode ControlMode = EditorEngine->GetLevelEditor()->GetActiveViewportClientData().GetControlMode();
            if (ControlMode == CoordiMode::CDM_LOCAL)
            {
                // TODO: 임시로 RootComponent의 정보로 사용
                SetActorRotation(PickedActor->GetActorRotation());
            }
            else if (ControlMode == CoordiMode::CDM_WORLD)
            {
                SetActorRotation(FRotator(0.0f, 0.0f, 0.0f));
            }
        }
    }
}
