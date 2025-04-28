#include "AGamePlayer.h"
#include "Camera/CameraComponent.h"
AGamePlayer::AGamePlayer()
{
    //Camera = AddComponent<UCameraComponent>(EComponentOrigin::Constructor);
}

AGamePlayer::AGamePlayer(const AGamePlayer& Other) : Super(Other)
{
}

void AGamePlayer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    Input();
}

UObject* AGamePlayer::Duplicate() const
{
    AGamePlayer* NewActor = FObjectFactory::ConstructObjectFrom<AGamePlayer>(this);
    NewActor->DuplicateSubObjects(this);
    return NewActor;
}

void AGamePlayer::DuplicateSubObjects(const UObject* Source)
{
    Super::DuplicateSubObjects(Source);
    AGamePlayer* Origin = Cast<AGamePlayer>(Source);
}

void AGamePlayer::PostDuplicate()
{
    Super::PostDuplicate();
}

void AGamePlayer::Input()
{
    if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
    {
        if (!bLeftMouseDown)
        {
            bLeftMouseDown = true;
            UE_LOG(LogLevel::Display, "GamePlayer Left Mouse Click");
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
            UE_LOG(LogLevel::Display, "GamePlayer Right Mouse Click");
        }
    }
    else
    {
        if (bRightMouseDown) bRightMouseDown = false;
    }

    if (GetAsyncKeyState('W') & 0x8000)
    {
        if (!bWkeyDown) bWkeyDown = true;
        UE_LOG(LogLevel::Display, "GamePlayer W Button Down");
    }
    else
    {
        if (bWkeyDown) bWkeyDown = false;
    }

    if (GetAsyncKeyState('A') & 0x8000)
    {
        if (!bAkeyDown) bAkeyDown = true;
        UE_LOG(LogLevel::Display, "GamePlayer A Button Down");
    }
    else
    {
        if (bAkeyDown) bAkeyDown = false;
    }

    if (GetAsyncKeyState('S') & 0x8000)
    {
        if (!bSkeyDown) bSkeyDown = true;
        UE_LOG(LogLevel::Display, "GamePlayer S Button Down");
    }
    else
    {
        if (bSkeyDown) bSkeyDown = false;
    }

    if (GetAsyncKeyState('D') & 0x8000)
    {
        if (!bDkeyDown) bDkeyDown = true;
        UE_LOG(LogLevel::Display, "GamePlayer D Button Down");
    }
    else
    {
        if (bDkeyDown) bDkeyDown = false;
    }
}
