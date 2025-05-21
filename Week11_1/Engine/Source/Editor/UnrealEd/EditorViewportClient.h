#pragma once
#include <SlateCore/Input/Events.h>

#include "Define.h"
#include "EngineBaseTypes.h"
#include "ViewportClient.h"
#include "Container/Map.h"
#include "Container/String.h"
#include "HAL/PlatformType.h"
#include "Math/Matrix.h"
#include "Math/Vector.h"
#include "UObject/ObjectMacros.h"

#define MIN_ORTHOZOOM				1.0							/* 2D ortho viewport zoom >= MIN_ORTHOZOOM */
#define MAX_ORTHOZOOM				1e25	

class FRect;
class AActor;
class USceneComponent;

struct FViewportCameraTransform
{
public:
    FVector GetForwardVector() const;
    FVector GetRightVector() const;
    FVector GetUpVector() const;

public:
    FViewportCameraTransform();

    /** Sets the transform's location */
    void SetLocation(const FVector& Position)
    {
        ViewLocation = Position;
    }

    /** Sets the transform's rotation */
    void SetRotation(const FVector& Rotation)
    {
        ViewRotation = Rotation;
    }

    /** Sets the location to look at during orbit */
    void SetLookAt(const FVector& InLookAt)
    {
        LookAt = InLookAt;
    }

    /** Set the ortho zoom amount */
    void SetOrthoZoom(float InOrthoZoom)
    {
        assert(InOrthoZoom >= MIN_ORTHOZOOM && InOrthoZoom <= MAX_ORTHOZOOM);
        OrthoZoom = InOrthoZoom;
    }

    /** Check if transition curve is playing. */
 /*    bool IsPlaying();*/

    /** @return The transform's location */
    FORCEINLINE const FVector& GetLocation() const { return ViewLocation; }

    /** @return The transform's rotation */
    FORCEINLINE const FVector& GetRotation() const { return ViewRotation; }

    /** @return The look at point for orbiting */
    FORCEINLINE const FVector& GetLookAt() const { return LookAt; }

    /** @return The ortho zoom amount */
    FORCEINLINE float GetOrthoZoom() const { return OrthoZoom; }

public:
    /** Current viewport Position. */
    FVector	ViewLocation;
    /** Current Viewport orientation; valid only for perspective projections. */
    FVector ViewRotation;
    FVector	DesiredLocation;
    /** When orbiting, the point we are looking at */
    FVector LookAt;
    /** Viewport start location when animating to another location */
    FVector StartLocation;
    /** Ortho zoom amount */
    float OrthoZoom;
};

class FEditorViewportClient : public FViewportClient
{
public:
    FEditorViewportClient();
    ~FEditorViewportClient() override;

    FEditorViewportClient(const FEditorViewportClient&) = delete;
    FEditorViewportClient(FEditorViewportClient&&) = delete;
    FEditorViewportClient& operator=(const FEditorViewportClient&) = delete;
    FEditorViewportClient& operator=(FEditorViewportClient&&) = delete;

    virtual void Draw(FViewport* Viewport) override;
    UWorld* GetWorld() const override;
    virtual void Initialize(HWND InOwnerWindow, uint32 InViewportIndex, UWorld* World) override;
    
    void Tick(float DeltaTime);
    void Release() const;
    void UpdateEditorCameraMovement(float DeltaTime);

    // @todo MultiViewport 떄문에 AppWnd를 인자로 받는중이다. 제거할 방안
    void InputKey(HWND AppWnd, const FKeyEvent& InKeyEvent);
    void UpdateCameraInputKeyState(const FKeyEvent& InKeyEvent);
    void MouseMove(const FPointerEvent& InMouseEvent);

    void ResizeViewport(FRect Top, FRect Bottom, FRect Left, FRect Right);
    void ResizeViewport(FRect InRect);

    bool IsSelected(FVector2D Point) const;
    
protected:
    /** Camera speed setting */
    int32 CameraSpeedSetting = 1;
    /** Camera speed scalar */

public:
    float CameraSpeedScalar = 1.0f;
    float GridSize;

public: 
    FViewport* GetViewport() const { return Viewport; }

    const D3D11_VIEWPORT& GetD3DViewport() const;
    
    uint32 GetViewportIndex() const { return ViewportIndex; }
    void SetViewportIndex(const uint32 InIndex) { ViewportIndex = InIndex; }
    
private:
    FViewport* Viewport;
    uint32 ViewportIndex;
    
public:
    //카메라
    /** Viewport camera transform data for perspective viewports */
    FViewportCameraTransform		ViewTransformPerspective;
    FViewportCameraTransform        ViewTransformOrthographic;
    //override camera
    USceneComponent* OverrideComponent = nullptr;
    // 카메라 정보 
    float ViewFOV = 60.0f;
    /** Viewport's stored horizontal field of view (saved in ini files). */
    float FOVAngle = 60.0f;
    float AspectRatio;
    float NearPlane = 0.1f;
    float FarPlane = 1000.0f;

    static FVector Pivot;
    static float OrthoSize;
    
    ELevelViewportType ViewportType;
    uint64 ShowFlag;
    EViewModeIndex ViewMode;

    FMatrix View;
    FMatrix InvView;
    FMatrix Projection;

    // Cascade Shadow Map
private:
    float CascadeSplits[CASCADE_COUNT]; // 카스케이드 경계 저장
    FVector CascadeCorners[CASCADE_COUNT][8];

    
    void CalculateCascadeSplits(float NearClip, float FarClip);
    void CalculateFrustumCorners(UINT CascadeIndex);
    void CalculateFrustumCornersInCameraSpace(float NearDist, float FarDist, int CascadeIndex);


    //Test
    TArray<AActor*> DebugCube;

public: //Camera Movement
    FVector* GetCascadeCorner(const UINT CascadeIndex) { return CascadeCorners[CascadeIndex]; }
    float GetCascadeSplit(const UINT CascadeIndex) const { return CascadeSplits[CascadeIndex]; }
    void UpdateCascadeShadowArea();
    void CameraMoveForward(float Value);
    void CameraMoveRight(float Value);
    void CameraMoveUp(float Value);
    void CameraRotateYaw(float Value);
    void CameraRotatePitch(float Value);
    void PivotMoveRight(float Value);
    void PivotMoveUp(float Value);

    void SetViewMatrix(FMatrix& InViewMatrix);
    
    FMatrix& GetViewMatrix() { return  View; }
    FMatrix& GetInvViewMatrix() { return  InvView; }
    FMatrix& GetProjectionMatrix() { return Projection; }
    void UpdateViewMatrix();
    void UpdateProjectionMatrix();

    void SetViewFOV(const float ViewFov) {ViewFOV = ViewFov;}
    float GetViewFOV() const { return ViewFOV; }
    void SetNearClip(const float NewNearClip) { NearPlane = NewNearClip; }
    float GetNearClip() const { return NearPlane; }
    void SetFarClip(const float NewFarClip) {FarPlane = NewFarClip;}
    float GetFarClip() const { return FarPlane; }
    float GetAspectRatio() const { return AspectRatio; }
    
    bool IsOrtho() const;
    bool IsPerspective() const;
    ELevelViewportType GetViewportType() const;
    void SetViewportType(ELevelViewportType InViewportType);
    void UpdateOrthoCameraLoc();
    EViewModeIndex GetViewMode() const { return ViewMode; }
    void SetViewMode(const EViewModeIndex NewMode) { ViewMode = NewMode; }
    uint64 GetShowFlag() const { return ShowFlag; }
    void SetShowFlag(const uint64 NewMode) { ShowFlag = NewMode; }

    // 마우스 우클릭 상태 제어 메소드
    void SetRightMouseDown(const bool bIsDown) { bRightMouseDown = bIsDown; }
    bool GetRightMouseDown() const { return bRightMouseDown; }

    //camera overriding
    USceneComponent* GetOverrideComponent() const { return OverrideComponent; }
    void SetOverrideComponent(USceneComponent* NewComp) { OverrideComponent = NewComp; }

    static float GetOrthoSize() { return OrthoSize; }
    static void SetOrthoSize(float InValue);

private: // Input
    POINT LastMousePos;
    bool bRightMouseDown = false;
    bool bLCtrlDown = false;

    //Temp
    TSet<EKeys::Type> CameraInputPressedKeys;
    
    TSet<EKeys::Type> PressedKeys;
    
public:
    void LoadConfig(const TMap<FString, FString>& Config);
    void SaveConfig(TMap<FString, FString>& Config) const;
private:
    TMap<FString, FString> ReadIniFile(const FString& FilePath) const;
    void WriteIniFile(const FString& FilePath, const TMap<FString, FString>& Config) const;
	
public:
    float GetCameraSpeedScalar() const { return CameraSpeedScalar; };
    void SetCameraSpeed(float Value);

private:
    template <typename T>
    T GetValueFromConfig(const TMap<FString, FString>& Config, const FString& Key, T DefaultValue) {
        if (const FString* Value = Config.Find(Key))
        {
            std::istringstream iss(**Value);
            T NewValue;
            if (iss >> NewValue)
            {
                return NewValue;
            }
        }
        return DefaultValue;
    }
};

