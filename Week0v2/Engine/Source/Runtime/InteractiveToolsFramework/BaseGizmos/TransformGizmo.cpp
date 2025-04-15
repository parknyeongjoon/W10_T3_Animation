#include "TransformGizmo.h"
#include "GizmoArrowComponent.h"
#include "Define.h"
#include "GizmoCircleComponent.h"
#include "Actors/Player.h"
#include "GizmoRectangleComponent.h"
#include "Engine/World.h"
#include "Engine/FLoaderOBJ.h"

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
        AddComponent<USceneComponent>()
    );

    UGizmoArrowComponent* locationX = AddComponent<UGizmoArrowComponent>();
    locationX->SetStaticMesh(FManagerOBJ::GetStaticMesh(L"gizmo_loc_x.obj"));
	locationX->SetupAttachment(RootComponent);
    locationX->SetGizmoType(UGizmoBaseComponent::ArrowX);
	ArrowArr.Add(locationX);

    UGizmoArrowComponent* locationY = AddComponent<UGizmoArrowComponent>();
    locationY->SetStaticMesh(FManagerOBJ::GetStaticMesh(L"gizmo_loc_y.obj"));
    locationY->SetupAttachment(RootComponent);
    locationY->SetGizmoType(UGizmoBaseComponent::ArrowY);
    ArrowArr.Add(locationY);

    UGizmoArrowComponent* locationZ = AddComponent<UGizmoArrowComponent>();
    locationZ->SetStaticMesh(FManagerOBJ::GetStaticMesh(L"gizmo_loc_z.obj"));
    locationZ->SetupAttachment(RootComponent);
    locationZ->SetGizmoType(UGizmoBaseComponent::ArrowZ);
    ArrowArr.Add(locationZ);

    UGizmoRectangleComponent* ScaleX = AddComponent<UGizmoRectangleComponent>();
    ScaleX->SetStaticMesh(FManagerOBJ::GetStaticMesh(L"gizmo_scale_x.obj"));
    ScaleX->SetupAttachment(RootComponent);
    ScaleX->SetGizmoType(UGizmoBaseComponent::ScaleX);
    RectangleArr.Add(ScaleX);

    UGizmoRectangleComponent* ScaleY = AddComponent<UGizmoRectangleComponent>();
    ScaleY->SetStaticMesh(FManagerOBJ::GetStaticMesh(L"gizmo_scale_y.obj"));
    ScaleY->SetupAttachment(RootComponent);
    ScaleY->SetGizmoType(UGizmoBaseComponent::ScaleY);
    RectangleArr.Add(ScaleY);

    UGizmoRectangleComponent* ScaleZ = AddComponent<UGizmoRectangleComponent>();
    ScaleZ->SetStaticMesh(FManagerOBJ::GetStaticMesh(L"gizmo_scale_z.obj"));
    ScaleZ->SetupAttachment(RootComponent);
    ScaleZ->SetGizmoType(UGizmoBaseComponent::ScaleZ);
    RectangleArr.Add(ScaleZ);

    UGizmoCircleComponent* CircleX = AddComponent<UGizmoCircleComponent>();
    CircleX->SetStaticMesh(FManagerOBJ::GetStaticMesh(L"gizmo_rot_x.obj"));
    CircleX->SetupAttachment(RootComponent);
    CircleX->SetGizmoType(UGizmoBaseComponent::CircleX);
    CircleArr.Add(CircleX);

    UGizmoCircleComponent* CircleY = AddComponent<UGizmoCircleComponent>();
    CircleY->SetStaticMesh(FManagerOBJ::GetStaticMesh(L"gizmo_rot_y.obj"));
    CircleY->SetupAttachment(RootComponent);
    CircleY->SetGizmoType(UGizmoBaseComponent::CircleY);
    CircleArr.Add(CircleY);

    UGizmoCircleComponent* CircleZ = AddComponent<UGizmoCircleComponent>();
    CircleZ->SetStaticMesh(FManagerOBJ::GetStaticMesh(L"gizmo_rot_z.obj"));
    CircleZ->SetupAttachment(RootComponent);
    CircleZ->SetGizmoType(UGizmoBaseComponent::CircleZ);
    CircleArr.Add(CircleZ);
}

void UTransformGizmo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (!GetWorld()->GetSelectedActors().IsEmpty())
    {
        const AActor* PickedActor = *GetWorld()->GetSelectedActors().begin();
        if (PickedActor == nullptr)
            return;
        SetActorLocation(PickedActor->GetActorLocation());
        if (GetWorld()->GetEditorPlayer()->GetCoordiMode() == CoordiMode::CDM_LOCAL)
        {
            // TODO: 임시로 RootComponent의 정보로 사용
            SetActorRotation(PickedActor->GetActorRotation());
        }
        else if (GetWorld()->GetEditorPlayer()->GetCoordiMode() == CoordiMode::CDM_WORLD)
            SetActorRotation(FVector(0.0f, 0.0f, 0.0f));
    }
}
