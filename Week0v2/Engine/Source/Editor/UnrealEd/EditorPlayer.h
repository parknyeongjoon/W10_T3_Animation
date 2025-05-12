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

    static void Initialize();

    static bool PickGizmo(ControlMode ControlMode, UWorld* World, const FVector& PickPosition);
    void PickActor(UWorld* World, const FVector& PickPosition) const;

    void MultiSelectingStart();
    void MultiSelectingEnd(UWorld* World);
    void MakeMulitRect() const;

    static void ScreenToViewSpace(int ScreenX, int ScreenY, const FMatrix& viewMatrix, const FMatrix& ProjectionMatrix, FVector& RayOrigin);

    void PickedObjControl(ControlMode ControlMode, CoordiMode CoordiMode, UWorld* World);
    bool GetMultiSelecting() const { return bMultiSeleting; }
    void SetAlreadyDup(const bool InbAlreadyDup) { bAlreadyDup = InbAlreadyDup; }
private:
    static int RayIntersectsObject(const FVector& PickPosition, USceneComponent* Component, float& HitDistance, int& IntersectCount);

    void ControlRotation(CoordiMode CoordiMode, UWorld* World, USceneComponent* pObj, const UGizmoBaseComponent* Gizmo, int32 DeltaX, int32 DeltaY);
    void ControlTranslation(CoordiMode CoordiMode, UWorld* World, USceneComponent* pObj, const UGizmoBaseComponent* Gizmo, int32 DeltaX, int32 DeltaY);
    static void ControlScale(USceneComponent* pObj, const UGizmoBaseComponent* Gizmo, int32 DeltaX, int32 DeltaY);
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
};

