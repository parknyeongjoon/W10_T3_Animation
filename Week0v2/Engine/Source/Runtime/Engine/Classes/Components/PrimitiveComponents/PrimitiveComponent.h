#pragma once
#include "Engine/Source/Runtime/Engine/Classes/Components/SceneComponent.h"
#include "Serialization/Archive.h"

struct HitResult;

struct FPrimitiveComponentInfo : FSceneComponentInfo
{
    DECLARE_ACTORCOMPONENT_INFO(FPrimitiveComponentInfo);
    FVector ComponentVelocity;
    FName VBIBTopologyMappingName;

    FPrimitiveComponentInfo()
        : FSceneComponentInfo()
        , ComponentVelocity(FVector::ZeroVector)
        , VBIBTopologyMappingName(TEXT(""))
    {
        InfoType = TEXT("FPrimitiveComponentInfo");
        ComponentClass = TEXT("UPrimitiveComponent");
    }

    virtual void Serialize(FArchive& ar) const override
    {
        FSceneComponentInfo::Serialize(ar);
        ar << ComponentVelocity << VBIBTopologyMappingName;
    }

    virtual void Deserialize(FArchive& ar) override
    {
        FSceneComponentInfo::Deserialize(ar);
        ar >> ComponentVelocity >> VBIBTopologyMappingName;
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
    virtual UObject* Duplicate() override;
    virtual void DuplicateSubObjects(const UObject* Source) override;
    virtual void PostDuplicate() override;

    // Physics - Collision
    bool IsGenerateOverlapEvents() const { return bGenerateOverlapEvents; }
    void SetGenerateOverlapEvents(bool bInGenerateOverlapEvents) { bGenerateOverlapEvents = bInGenerateOverlapEvents; }

    virtual void NotifyHit(HitResult Hit) const;
    virtual void NotifyBeginOverlap(const UPrimitiveComponent* OtherComponent) const;
    virtual void NotifyEndOverlap(const UPrimitiveComponent* OtherComponent) const;

    bool MoveComponent(const FVector& Delta) override;
    FVector ComponentVelocity;

public:
    
    std::unique_ptr<FActorComponentInfo> GetComponentInfo() override;
    virtual void SaveComponentInfo(FActorComponentInfo& OutInfo) override;
    virtual void LoadAndConstruct(const FActorComponentInfo& Info);
public:
    FName GetVBIBTopologyMappingName() const { return VBIBTopologyMappingName; }
protected:
    FName VBIBTopologyMappingName;

private:
    bool bGenerateOverlapEvents = true;
};

