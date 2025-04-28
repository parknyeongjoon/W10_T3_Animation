#pragma once
#include "Components/SceneComponent.h"

class UCameraComponent : public USceneComponent
{
    DECLARE_CLASS(UCameraComponent, USceneComponent)

public:
    UCameraComponent();
    virtual ~UCameraComponent() override;

    virtual void InitializeComponent() override;
    virtual void TickComponent(float DeltaTime) override;

    void Input();
    void MoveForward(float _Value);
    void MoveRight(float _Value);
    void MoveUp(float _Value);
    void RotateYaw(float _Value);
    void RotatePitch(float _Value);

    bool IsCameraMode() const { return bRightMouseDown; }

private:
    float mouseSpeed = 0.25f;
    POINT lastMousePos;
    bool bRightMouseDown = false;

    FVector zAxis; // DirectX는 LH이므로 -z가 아니라 +z 사용
    FVector xAxis;
    FVector yAxis;
    float FOV;
    float nearClip = 0.1f;
    float farClip = 1000.f;
public:
    void SetFOV(float _fov) { FOV = _fov; }
    float& GetFOV() { return FOV; }
    void SetNearClip(float _Value) { nearClip = _Value; }
    float& GetNearClip() { return nearClip; }
    void SetFarClip(float _Value) { farClip = _Value; }
    float& GetFarClip() { return farClip; }
    void SetMouseSpeed(float _Value) { mouseSpeed = _Value; }
    float GetMouseSpeed() const { return mouseSpeed; }

    FMatrix GetViewMatrix() const;
    FMatrix GetProjectionMatrix() const;
};
