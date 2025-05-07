#pragma once
#include "SceneComponent.h"

class USphereShapeComponent;

class USpringArmComponent : public USceneComponent
{
    DECLARE_CLASS(USpringArmComponent, USceneComponent)

public:
    USpringArmComponent();
    USpringArmComponent(const USpringArmComponent& Other);
    virtual ~USpringArmComponent() override;

    void InitializeComponent() override;
    void BeginPlay() override;
    void TickComponent(float DeltaTime) override;
    void DestroyComponent() override;

    virtual UObject* Duplicate(UObject* InOuter) override;
    void DuplicateSubObjects(const UObject* Source, UObject* InOuter) override;
    void PostDuplicate() override;

private:


public:
#pragma region Getter & Setter
    USceneComponent* GetTargetComponent() const { return TargetComponent; }
    void SetTargetComponent(USceneComponent* InTargetComponent);

    // 1. Offset
    float GetTargetArmLength() const { return TargetArmLength; }
    void SetTargetArmLength(float InTargetArmLength) { ChangeTargetArmLength = InTargetArmLength; }
    FVector GetSocketOffset() const { return SocketOffset; }
    void SetSocketOffset(const FVector& InSocketOffset) { SocketOffset = InSocketOffset; }
    FVector GetTargetOffset() const { return TargetOffset; }
    void SetTargetOffset(const FVector& InTargetOffset) { TargetOffset = InTargetOffset; }

    // 2. Camera Collision
    bool IsUsingCollisionTest() const { return bUseCollisionTest; }
    void SetUseCollisionTest(bool InUseCollisionTest) { bUseCollisionTest = InUseCollisionTest; }
    float GetProbeSize() const { return ProbeSize; }
    void SetProbeSize(float InProbeSize) 
    { 
        ProbeSize = FMath::Clamp<float>(InProbeSize, 0, 50); 
    }

    // 3. Lag
    bool isActiveCameraLag() const { return bActiveCameraLag; }
    void SetActiveCameraLag(bool InActiveCameraLag) { bActiveCameraLag = InActiveCameraLag; }
    float GetCameraLagSpeed() const { return CameraLagSpeed; }
    void SetCameraLagSpeed(float InCameraLagSpeed) { CameraLagSpeed = InCameraLagSpeed; }
    float GetCameraLagMaxDistance() const { return CameraLagMaxDistance; }
    void SetCameraLagMaxDistance(float InCameraLagMaxDistance) 
    { 
        CameraLagMaxDistance = FMath::Clamp<float>(InCameraLagMaxDistance, 0, 100); 
    }

    bool isActiveCameraRotationLag() const { return bActiveCameraRotationLag; }
    void SetActiveCameraRotationLag(bool InActiveCameraRotationLag) { bActiveCameraRotationLag = InActiveCameraRotationLag; }
    float GetCameraRotationLagSpeed() const { return CameraRotationLagSpeed; }
    void SetCameraRotationLagSpeed(float InCameraRotationLagSpeed) { CameraRotationLagSpeed = InCameraRotationLagSpeed; }
    float GetCameraRotationLagMaxAngle() const { return CameraRotationLagMaxAngle; }
    void SetCameraRotationLagMaxAngle(float InCameraRotationLagMaxAngle) 
    { 
        CameraRotationLagMaxAngle = FMath::Clamp<float>(InCameraRotationLagMaxAngle, 0, 100); 
    }

    // 4. Camera Setting
    bool IsUsingPawnControlRotation() const { return bUsePawnControlRotation; }
    void SetUsePawnControlRotation(bool InUsePawnControlRotation) { bUsePawnControlRotation = InUsePawnControlRotation; }
    bool IsInheritingPitch() const { return bInheritPitch; }
    void SetInheritPitch(bool InInheritPitch) { bInheritPitch = InInheritPitch; }
    bool IsInheritingYaw() const { return bInheritYaw; }
    void SetInheritYaw(bool InInheritYaw) { bInheritYaw = InInheritYaw; }
    bool IsInheritingRoll() const { return bInheritRoll; }
    void SetInheritRoll(bool InInheritRoll) { bInheritRoll = InInheritRoll; }
#pragma endregion

private:
    USphereShapeComponent* Probe = nullptr;
    USceneComponent* TargetComponent = nullptr;

    // 1. Offset
    float ChangeTargetArmLength = 0;
    float TargetArmLength = 2;
    FVector SocketOffset = FVector(0, 0, 0);
    FVector TargetOffset = FVector(0, 0, 0);

    // 2. Camera Collision
    bool bUseCollisionTest = true;
    float ProbeSize = 0.5f;
    //float ProbeChannel = 0.5f;

    // 3. Lag
    bool bActiveCameraLag = false;
    float CameraLagSpeed = 0.5f;
    float CameraLagMaxDistance = 50.f;

    bool bActiveCameraRotationLag = false;
    float CameraRotationLagSpeed = 0.5f;
    float CameraRotationLagMaxAngle = 20.f;

    // 4. Camera Setting
    bool bUsePawnControlRotation = false;
    bool bInheritPitch = true;
    bool bInheritYaw = true;
    bool bInheritRoll = true;
};

