#include "AGPlayer.h"
#include "Camera/CameraComponent.h"
#include "EditorEngine.h"
#include "LevelEditor/SLevelEditor.h"
#include "UnrealEd/EditorViewportClient.h"
#include "Engine/World.h"
#include "AGBullet.h"
#include "APlayerCameraManager.h"
#include "Camera/GunRecoilShake.h"
#include "Curves/CurveFloat.h"
AGPlayer::AGPlayer()
{
    //Camera = AddComponent<UCameraComponent>(EComponentOrigin::Constructor);
}

AGPlayer::AGPlayer(const AGPlayer& Other) : Super(Other)
{
}

void AGPlayer::BeginPlay()
{
    Super::BeginPlay();

    CURSORINFO cursorInfo = { sizeof(CURSORINFO) };
    GetCursorInfo(&cursorInfo);
    if (cursorInfo.flags == CURSOR_SHOWING)
    {
        ShowCursor(FALSE);
        bShowCursor = false;
    }

    GetCursorPos(&lastMousePos);
    UCameraComponent* Camera = GetComponentByClass<UCameraComponent>();
    FTViewTarget ViewTarget;
    ViewTarget.Target = this;
    ViewTarget.ViewInfo = FViewInfo(Camera->GetWorldLocation(), Camera->GetWorldRotation(), Camera->GetFOV());
    for (auto& Actor : GEngine->GetWorld()->GetActors())
    {
        if (APlayerCameraManager* APCM = Cast<APlayerCameraManager>(Actor))
        {
            APCM->AssignViewTarget(ViewTarget);
            PlayerCameraManager = APCM;
            break;
        }
    }
    GEngine->GetLevelEditor()->GetActiveViewportClient()->SetOverrideComponent(Camera);
    UE_LOG(LogLevel::Display, "AGamePlayer Begin Play");
}

void AGPlayer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    Input(DeltaTime);
}

void AGPlayer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    CURSORINFO cursorInfo = { sizeof(CURSORINFO) };
    GetCursorInfo(&cursorInfo);
    if (cursorInfo.flags != CURSOR_SHOWING)
    {
        ShowCursor(TRUE);
        bShowCursor = true;
    }
    GEngine->GetLevelEditor()->GetActiveViewportClient()->SetOverrideComponent(nullptr);
    UE_LOG(LogLevel::Display, "AGamePlayer End Play");
}

UObject* AGPlayer::Duplicate() const
{
    AGPlayer* NewActor = FObjectFactory::ConstructObjectFrom<AGPlayer>(this);
    NewActor->DuplicateSubObjects(this);
    return NewActor;
}

void AGPlayer::DuplicateSubObjects(const UObject* Source)
{
    Super::DuplicateSubObjects(Source);
    AGPlayer* Origin = Cast<AGPlayer>(Source);
}

void AGPlayer::PostDuplicate()
{
    Super::PostDuplicate();
}

void AGPlayer::Input(float DeltaTime)
{
    if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
    {
        if (!bLeftMouseDown)
        {
            bLeftMouseDown = true;
            AGBullet* bullet = GEngine->GetWorld()->SpawnActor<AGBullet>();
            bullet->Fire(GetActorLocation(), GetActorForwardVector(), 50);

            if (PlayerCameraManager)
            {
                UGunRecoilShake* Shake = FObjectFactory::ConstructObject<UGunRecoilShake>();
                Shake->Duration = 0.5f;
                
                // Pitch 튐 → 복귀 곡선
                UCurveFloat* PitchCurve = FObjectFactory::ConstructObject<UCurveFloat>();
                PitchCurve->AddKey(0.0f, 0.0f);
                PitchCurve->AddKey(0.1f, 10.f);
                PitchCurve->AddKey(0.5f, 0.0f);

                Shake->PitchCurve = PitchCurve;

                PlayerCameraManager->StartCameraShake(Shake);
            }
        }
    }
    else
    {
        if (bLeftMouseDown) bLeftMouseDown = false;
    }

    if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
    {
        if (!bRightMouseDown)
        {
            bRightMouseDown = true;
        }
    }
    else
    {
        if (bRightMouseDown) bRightMouseDown = false;
    }

    if (GetAsyncKeyState(VK_SPACE) & 0x8000)
    {
        if (!bSpacePressedLastFrame)
        {
            bShowCursor = !bShowCursor;
            ShowCursor(bShowCursor ? TRUE : FALSE);

            bSpacePressedLastFrame = true;
        }
    }
    else
    {
        bSpacePressedLastFrame = false;
    }

    if (!bShowCursor) // 커서 숨김 상태일 때만 마우스 회전
    {
        POINT currentMousePos;
        GetCursorPos(&currentMousePos);




        int32 deltaX = currentMousePos.x - lastMousePos.x;
        int32 deltaY = currentMousePos.y - lastMousePos.y;

        FRotator Rotation = GetActorRotation();
        Rotation.Yaw += deltaX * YawSpeed;    // 좌우 마우스 이동 -> Yaw 회전
        Rotation.Pitch -= deltaY * PitchSpeed;  // 위아래 마우스 이동 -> Pitch 회전 (보통 위로 올리면 Pitch 감소)

        // Pitch 클램프
        if (Rotation.Pitch > 89.0f) Rotation.Pitch = 89.0f;
        if (Rotation.Pitch < -89.0f) Rotation.Pitch = -89.0f;

        SetActorRotation(Rotation);

        RECT rect;
        GetClientRect(GetActiveWindow(), &rect);
        POINT center;
        center.x = (rect.right - rect.left) / 2;
        center.y = (rect.bottom - rect.top) / 2;
        ClientToScreen(GetActiveWindow(), &center);
        SetCursorPos(center.x, center.y);
        // 다음 프레임을 위해 현재 마우스 위치 저장
        lastMousePos = center;
    }

    FVector MoveDirection = FVector::ZeroVector;
    if (GetAsyncKeyState('W') & 0x8000) MoveDirection += GetActorForwardVector();
    if (GetAsyncKeyState('S') & 0x8000) MoveDirection -= GetActorForwardVector();
    if (GetAsyncKeyState('D') & 0x8000) MoveDirection += GetActorRightVector();
    if (GetAsyncKeyState('A') & 0x8000) MoveDirection -= GetActorRightVector();
    
    if (!MoveDirection.IsNearlyZero())
    {
        MoveDirection.z = 0.0f;
        MoveDirection.Normalize();
        SetActorLocation(GetActorLocation() + MoveDirection * MoveSpeed * DeltaTime);
    }
}
