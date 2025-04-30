#pragma once
#include "Engine/StaticMeshActor.h"

class UCameraComponent;
class APlayerCameraManager;
class AGPlayer :
    public AActor
{
    DECLARE_CLASS(AGPlayer,AActor)
public:
    AGPlayer();
    AGPlayer(const AGPlayer& Other);
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UObject* Duplicate() const override;
    void DuplicateSubObjects(const UObject* Source) override;
    void PostDuplicate() override;
private:
    void Input(float DeltaTime);
private:
    float YawSpeed = 0.03f;
    float PitchSpeed = 0.03f;
    float MoveSpeed = 0.02f;
    bool bLeftMouseDown = false;
    bool bRightMouseDown = false;
    bool bShowCursor = false;
    bool bSpacePressedLastFrame = false;
    POINT lastMousePos;

private:
    APlayerCameraManager* PlayerCameraManager = nullptr;
};

