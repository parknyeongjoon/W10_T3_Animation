#pragma once
#include "Engine/Source/Runtime/Engine/Classes/Components/SceneComponent.h"
#include "Serialization/Archive.h"

struct FPrimitiveComponentInfo : FSceneComponentInfo
{
    DECLARE_ACTORCOMPONENT_INFO(FPrimitiveComponentInfo);

    FBoundingBox AABB;

    FPrimitiveComponentInfo()
        : FSceneComponentInfo()
        , AABB(FVector::ZeroVector, FVector::ZeroVector)
    {
        InfoType = TEXT("FPrimitiveComponentInfo");
        ComponentType = TEXT("UPrimitiveComponent");
    }

    virtual void Copy(FActorComponentInfo& Other) override
    {
        FSceneComponentInfo::Copy(Other);
        FPrimitiveComponentInfo& OtherPrimitive = static_cast<FPrimitiveComponentInfo&>(Other);
        AABB = OtherPrimitive.AABB;
    }

    virtual void Serialize(FArchive& ar) const override
    {
        FSceneComponentInfo::Serialize(ar);
        ar << AABB;
    }

    virtual void Deserialize(FArchive& ar) override
    {
        FSceneComponentInfo::Deserialize(ar);
        ar >> AABB;
    }
};

class UPrimitiveComponent : public USceneComponent
{
    DECLARE_CLASS(UPrimitiveComponent, USceneComponent)

public:
    UPrimitiveComponent();
    UPrimitiveComponent(const UPrimitiveComponent& Other);
    virtual ~UPrimitiveComponent() override;

    virtual void InitializeComponent() override;
    virtual void TickComponent(float DeltaTime) override;
    virtual int CheckRayIntersection(FVector& rayOrigin, FVector& rayDirection, float& pfNearHitDistance);
    bool IntersectRayTriangle(
        const FVector& rayOrigin, const FVector& rayDirection,
        const FVector& v0, const FVector& v1, const FVector& v2, float& hitDistance
    );
    virtual UObject* Duplicate() const override;
    virtual void DuplicateSubObjects(const UObject* Source) override;
    virtual void PostDuplicate() override;

    bool MoveComponent(const FVector& Delta) override;
    FBoundingBox AABB;
    FVector ComponentVelocity;

public:
    virtual std::shared_ptr<FActorComponentInfo> GetActorComponentInfo() override;
    virtual void LoadAndConstruct(const FActorComponentInfo& Info);

    FBoundingBox GetBoundingBox() { return AABB; }

};

