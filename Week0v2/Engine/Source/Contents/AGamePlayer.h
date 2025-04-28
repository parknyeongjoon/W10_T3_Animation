#pragma once
#include "Engine/StaticMeshActor.h"

class UCameraComponent;
class AGamePlayer :
    public AActor
{
    DECLARE_CLASS(AGamePlayer,AActor)
public:
    AGamePlayer();
    AGamePlayer(const AGamePlayer& Other);
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UObject* Duplicate() const override;
    void DuplicateSubObjects(const UObject* Source) override;
    void PostDuplicate() override;
private:
    void Input(float DeltaTime);
private:
    float YawSpeed = 0.1f;
    float PitchSpeed = 0.1f;
    float MoveSpeed = 0.1f;
    bool bLeftMouseDown = false;
    bool bRightMouseDown = false;
    POINT lastMousePos;
};

