#pragma once
#include "GameFramework/Actor.h"

class UCameraComponent;
class AGamePlayer :
    public AActor
{
    DECLARE_CLASS(AGamePlayer,AActor)
public:
    AGamePlayer();
    AGamePlayer(const AGamePlayer& Other);
    virtual void Tick(float DeltaTime) override;

    UObject* Duplicate() const override;
    void DuplicateSubObjects(const UObject* Source) override;
    void PostDuplicate() override;
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

