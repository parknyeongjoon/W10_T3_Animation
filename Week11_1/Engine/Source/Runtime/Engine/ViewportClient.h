#pragma once


class FViewport;
class UWorld;
class FViewportClient
{
public:
    virtual ~FViewportClient() = default;

    // FViewport에서 발생하는 이벤트를 처리하는 가상 함수들
    //virtual void OnInput(const FInputEvent& Event) = 0;
    virtual void Draw(FViewport* Viewport) = 0;
    virtual void Initialize(HWND AppWnd, uint32 InViewportIndex, UWorld* World) = 0;
    virtual UWorld* GetWorld() const { return nullptr; }
    // FViewport에 대한 참조 (혹은 소유)

    // TODO 이렇게 하는게 맞으려나 모르겠다 Owner
    virtual void SetOwner(HWND InOwnerWindow) { OwnerWindow = InOwnerWindow; }
    virtual void SetWorld(UWorld* InWorld) { World = InWorld; }

    // 종속된 Window
    HWND OwnerWindow;
    UWorld* World;
};
    