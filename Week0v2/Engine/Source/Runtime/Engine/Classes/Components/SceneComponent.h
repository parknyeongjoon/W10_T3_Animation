#pragma once
#include "ActorComponent.h"
#include "Math/Quat.h"
#include "UObject/ObjectMacros.h"
#include "ActorComponentInfo.h"

struct FSceneComponentInfo : public FActorComponentInfo
{
    DECLARE_ACTORCOMPONENT_INFO(FSceneComponentInfo);
    
    FVector RelativeLocation;
    FVector RelativeRotation;
    FQuat QuatRotation;
    FVector RelativeScale3D;
    FBoundingBox AABB;

    FSceneComponentInfo()
        : FActorComponentInfo()
        , RelativeLocation(FVector::ZeroVector)
        , RelativeRotation(FVector::ZeroVector)
        , QuatRotation(FQuat::Identity())
        , RelativeScale3D(FVector::OneVector)
        , AABB(FBoundingBox(FVector::ZeroVector, FVector::ZeroVector))
    {
        InfoType = TEXT("FSceneComponentInfo");
        ComponentType = TEXT("USceneComponent");
    }

    virtual void Copy(FActorComponentInfo& Other) override
    {
        FActorComponentInfo::Copy(Other);
        FSceneComponentInfo& OtherScene = static_cast<FSceneComponentInfo&>(Other);
        OtherScene.RelativeLocation = RelativeLocation;
        OtherScene.RelativeRotation = RelativeRotation;
        OtherScene.QuatRotation = QuatRotation;
        OtherScene.RelativeScale3D = RelativeScale3D;
        OtherScene.AABB = AABB;
    }

    virtual void Serialize(FArchive& ar) const override
    {
        FActorComponentInfo::Serialize(ar);
        ar << RelativeLocation << RelativeRotation << QuatRotation << RelativeScale3D << AABB;
    }

    virtual void Deserialize(FArchive& ar) override
    {
        FActorComponentInfo::Deserialize(ar);
        ar >> RelativeLocation >> RelativeRotation >> QuatRotation >> RelativeScale3D >> AABB;
    }
};

class USceneComponent : public UActorComponent
{
    DECLARE_CLASS(USceneComponent, UActorComponent)

public:
    USceneComponent();
    USceneComponent(const USceneComponent& Other);
    virtual ~USceneComponent() override;

    virtual void InitializeComponent() override;
    virtual void TickComponent(float DeltaTime) override;
    virtual int CheckRayIntersection(FVector& rayOrigin, FVector& rayDirection, float& pfNearHitDistance);
    virtual FVector GetForwardVector();
    virtual FVector GetRightVector();
    virtual FVector GetUpVector();
    void AddLocation(FVector _added);
    void AddRotation(FVector _added);
    void AddScale(FVector _added);

    FBoundingBox GetBoundingBox() { return AABB; }
    void SetBoundingBox(const FBoundingBox& InAABB) { AABB = InAABB; }
protected:
    FVector RelativeLocation;
    FVector RelativeRotation;
    FQuat QuatRotation;
    FVector RelativeScale3D;

    USceneComponent* AttachParent = nullptr;
    TArray<USceneComponent*> AttachChildren;

    FBoundingBox AABB;
public:
    virtual FVector GetWorldRotation();
    FVector GetWorldScale();
    FVector GetWorldLocation();
    FVector GetLocalRotation();
    FQuat GetQuat() const { return QuatRotation; }

#define region
    FVector GetRelativeLocation() const { return RelativeLocation; }
    FVector GetRelativeRotation() const { return RelativeRotation; }
    FQuat GetRelativeQuat() const { return QuatRotation; }
    FVector GetRelativeScale() const { return RelativeScale3D; }
    FMatrix GetRelativeTransform() const;

    FVector GetComponentLocation() const;
    FVector GetComponentRotation() const;
    FQuat GetComponentQuat() const;
    FVector GetComponentScale() const;
    FMatrix GetComponentTransform() const;

    FMatrix GetComponentTranslateMatrix() const;
    FMatrix GetComponentRotationMatrix() const;
    FMatrix GetComponentScaleMatrix() const;
    
    FVector GetLocalScale() const { return RelativeScale3D; }
    FVector GetLocalLocation() const { return RelativeLocation; }


    void SetRelativeLocation(FVector _newLoc);
    void SetRelativeRotation(FVector _newRot);
    void SetRelativeQuat(FQuat _newRot);
    void SetRelativeScale(FVector _newScale);
    void SetupAttachment(USceneComponent* InParent);
    void DetachFromParent();
#define endregion
    void SetLocation(FVector _newLoc) { RelativeLocation = _newLoc; }
    virtual void SetRotation(FVector _newRot);
    void SetRotation(FQuat _newRot) { QuatRotation = _newRot; }
    void SetScale(FVector _newScale) { RelativeScale3D = _newScale; }

public:
    USceneComponent* GetAttachParent() const;
    void SetAttachParent(USceneComponent* InParent);
    TArray<USceneComponent*> GetAttachChildren() const { return AttachChildren; }
    virtual UObject* Duplicate() const override;
    virtual void DuplicateSubObjects(const UObject* Source) override;
    virtual void PostDuplicate();

public:
    virtual bool MoveComponent(const FVector& Delta) { return false; }
    virtual std::shared_ptr<FActorComponentInfo> GetActorComponentInfo() override;
    virtual void LoadAndConstruct(const FActorComponentInfo& Info) override;
};
