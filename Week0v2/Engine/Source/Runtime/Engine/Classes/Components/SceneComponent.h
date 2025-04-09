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

    FSceneComponentInfo()
        : FActorComponentInfo()
        , RelativeLocation(FVector::ZeroVector)
        , RelativeRotation(FVector::ZeroVector)
        , QuatRotation(FQuat::Identity())
        , RelativeScale3D(FVector::OneVector)
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
    }

    virtual void Serialize(FArchive& ar) const override
    {
        FActorComponentInfo::Serialize(ar);
        ar << RelativeLocation << RelativeRotation << QuatRotation << RelativeScale3D;
    }

    virtual void Deserialize(FArchive& ar) override
    {
        FActorComponentInfo::Deserialize(ar);
        ar >> RelativeLocation >> RelativeRotation >> QuatRotation >> RelativeScale3D;
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

protected:
    FVector RelativeLocation;
    FVector RelativeRotation;
    FQuat QuatRotation;
    FVector RelativeScale3D;

    USceneComponent* AttachParent = nullptr;
    TArray<USceneComponent*> AttachChildren;

public:
    virtual FVector GetWorldRotation();
    FVector GetWorldScale();
    FVector GetWorldLocation();
    FVector GetLocalRotation();
    FQuat GetQuat() const { return QuatRotation; }

    FVector GetLocalScale() const { return RelativeScale3D; }
    FVector GetLocalLocation() const { return RelativeLocation; }

    void SetLocation(FVector _newLoc) { RelativeLocation = _newLoc; }
    virtual void SetRotation(FVector _newRot);
    void SetRotation(FQuat _newRot) { QuatRotation = _newRot; }
    void SetScale(FVector _newScale) { RelativeScale3D = _newScale; }
    void SetupAttachment(USceneComponent* InParent);
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

private:
    class UTextUUID* uuidText = nullptr;

public:
};
