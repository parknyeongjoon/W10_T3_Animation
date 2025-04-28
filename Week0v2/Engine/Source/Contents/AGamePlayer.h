#pragma once
#include "GameFramework/Actor.h"

class UCameraComponent;
class AGamePlayer :
    public AActor
{
    DECLARE_CLASS(AGamePlayer,AActor)
public:
    AGamePlayer();
    virtual void Tick(float DeltaTime) override;
private:
    void Input();
private:
    UCameraComponent* Camera;
private:
    bool bLeftMouseDown = false;
    bool bRightMouseDown = false;
    bool bWkeyDown = false;
    bool bAkeyDown = false;
    bool bSkeyDown = false;
    bool bDkeyDown = false;
    POINT lastMousePos;
};

