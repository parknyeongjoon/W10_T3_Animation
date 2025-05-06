#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ObjectTypes.h"

struct FMatrix;
struct FVector;
class USceneComponent;
class UGizmoBaseComponent;

class UEditorPlayer : public UObject
{
    DECLARE_CLASS(UEditorPlayer, UObject)

    UEditorPlayer() = default;
    ~UEditorPlayer() = default;

    void Initialize();
    
    virtual void Tick();
    

    void Input();
    bool PickGizmo(UWorld* World, FVector& pickPosition);
    void PickActor(UWorld* World, const FVector& pickPosition);
    void AddControlMode();
    void AddCoordiMode();

private:
    int RayIntersectsObject(const FVector& pickPosition, USceneComponent* obj, float& hitDistance, int& intersectCount);
    void ScreenToViewSpace(int screenX, int screenY, const FMatrix& viewMatrix, const FMatrix& projectionMatrix, FVector& rayOrigin);
    void PickedObjControl(UWorld* World);
    void MultiSelectingStart();
    void MultiSelectingEnd(UWorld* World);
    void MakeMulitRect();

    void ControlRotation(UWorld* World, USceneComponent* pObj, UGizmoBaseComponent* Gizmo, int32 deltaX, int32 deltaY);
    void ControlTranslation(UWorld* World, USceneComponent* pObj, UGizmoBaseComponent* Gizmo, int32 deltaX, int32 deltaY);
    void ControlScale(USceneComponent* pObj, UGizmoBaseComponent* Gizmo, int32 deltaX, int32 deltaY);
    bool bLeftMouseDown = false;
    bool bRightMouseDown = false;
    bool bSpaceDown = false;
    bool bLCtrlDown = false;
    bool bDkeyDown = false;
    bool bLShiftDown = false;
    bool bLAltDown = false;
    bool bAlreadyDup = false;
    bool bMultiSeleting =false;
    
    POINT multiSelectingStartPos;
    POINT LastMousePosision;
    ControlMode cMode = CM_TRANSLATION;
    CoordiMode cdMode = CDM_WORLD;

public:
    void SetMode(ControlMode _Mode) { cMode = _Mode; }
    ControlMode GetControlMode() const { return cMode; }
    CoordiMode GetCoordiMode() const { return cdMode; }
};

