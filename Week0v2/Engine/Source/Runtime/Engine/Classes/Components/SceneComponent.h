#pragma once
#include "ActorComponent.h"
#include "Math/Quat.h"
#include "UObject/ObjectMacros.h"
#include "ActorComponentInfo.h"
#include "Math/Rotator.h"
#include "Misc/Guid.h"
#include <sol\sol.hpp>

#include "Define.h"

class USceneComponent;

struct FSceneComponentInfo : public FActorComponentInfo
{
    DECLARE_ACTORCOMPONENT_INFO(FSceneComponentInfo);

    FVector RelativeLocation;
    FRotator RelativeRotation;
    FVector RelativeScale3D;
    FBoundingBox AABB;

    
    FGuid ParentComponentID;
    

    FSceneComponentInfo()
        : FActorComponentInfo()
        , RelativeLocation(FVector::ZeroVector)
        , RelativeRotation(FVector::ZeroVector)
        , RelativeScale3D(FVector::OneVector)
        , AABB(FBoundingBox(FVector::ZeroVector, FVector::ZeroVector))
    {
        InfoType = TEXT("FSceneComponentInfo");
        //ComponentClass = TEXT("USceneComponent");
    }
    

    virtual void Serialize(FArchive& ar) const override
    {
        FActorComponentInfo::Serialize(ar);
        ar << RelativeLocation << RelativeRotation << RelativeScale3D << AABB;
        ar << ParentComponentID.A << ParentComponentID.B << ParentComponentID.C << ParentComponentID.D;
    }

    virtual void Deserialize(FArchive& ar) override
    {
        FActorComponentInfo::Deserialize(ar);
        ar >> RelativeLocation >> RelativeRotation >> RelativeScale3D >> AABB;
        ar >> ParentComponentID.A >> ParentComponentID.B >> ParentComponentID.C >> ParentComponentID.D;
    }
};

class USceneComponent : public UActorComponent
{
    DECLARE_CLASS(USceneComponent, UActorComponent)
    friend class FSceneComponentInfo;

public:
    USceneComponent();
    USceneComponent(const USceneComponent& Other);
    virtual ~USceneComponent() override;

    virtual void InitializeComponent() override;
    virtual void TickComponent(float DeltaTime) override;
    void DestroyComponent() override;

    virtual UObject* Duplicate() override;
    virtual void DuplicateSubObjects(const UObject* Source) override;
    
    virtual void PostDuplicate() {};

protected:
    FVector RelativeLocation;
    FRotator RelativeRotation;
    FVector RelativeScale;

    USceneComponent* AttachParent = nullptr;
    TArray<USceneComponent*> AttachChildren;

    FBoundingBox AABB;
    
public:
    FVector GetRelativeForwardVector() const;
    FVector GetRelativeRightVector() const;
    FVector GetRelativeUpVector() const;
    
    void AddRelativeLocation(const FVector& InAddValue);
    void AddRelativeRotation(const FRotator& InAddValue);
    void AddRelativeScale(const FVector& InAddValue);
    
    void SetRelativeLocation(const FVector& InLocation);
    void SetRelativeRotation(const FRotator& InRotation);
    void SetRelativeRotation(const FQuat& InQuat);
    void SetRelativeScale(const FVector& InScale);
    
    FVector GetRelativeLocation() const;
    FRotator GetRelativeRotation() const;
    FVector GetRelativeScale() const;

    FVector GetWorldForwardVector() const;
    FVector GetWorldRightVector() const;
    FVector GetWorldUpVector() const;

    FVector GetWorldLocation() const;
    FRotator GetWorldRotation() const;
    FVector GetWorldScale() const;

    void AddWorldLocation(const FVector& InAddValue);
    void AddWorldRotation(const FRotator& InAddValue);
    void AddWorldScale(const FVector& InAddValue);
    
    void SetWorldLocation(const FVector& InLocation);
    void SetWorldRotation(const FRotator& InRotation);
    void SetWorldRotation(const FQuat& InQuat);
    void SetWorldScale(const FVector& InScale);

    FMatrix GetScaleMatrix() const;
    FMatrix GetRotationMatrix() const;
    FMatrix GetTranslationMatrix() const;

    FMatrix GetWorldMatrix() const;
    FMatrix GetWorldRTMatrix() const;

public:
    void AttachToComponent(USceneComponent* InParent);
    void SetupAttachment(USceneComponent* InParent);
    void DetachFromParent();
    void SetAttachParent(USceneComponent* InParent);
    TArray<USceneComponent*> GetAttachChildren() const { return AttachChildren; }
    USceneComponent* GetAttachParent() const;

public:
    virtual int CheckRayIntersection(FVector& rayOrigin, FVector& rayDirection, float& pfNearHitDistance);

    FBoundingBox GetBoundingBox() const { return AABB; }
    void SetBoundingBox(const FBoundingBox& InAABB) { AABB = InAABB; }
    
public:
    virtual bool MoveComponent(const FVector& Delta) { return false; }
    std::unique_ptr<FActorComponentInfo> GetComponentInfo() override;
    virtual void SaveComponentInfo(FActorComponentInfo& OutInfo) override;
    virtual void LoadAndConstruct(const FActorComponentInfo& Info) override;

private:
    class UTextUUID* uuidText = nullptr;
private:
    FGuid PendingAttachParentID; // 로드 후 링크를 위해 임시 저장
public:
    FGuid GetPendingAttachParentID() const { return PendingAttachParentID; }
};
