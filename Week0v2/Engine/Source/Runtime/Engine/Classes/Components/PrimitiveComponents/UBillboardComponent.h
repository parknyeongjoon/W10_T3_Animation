#pragma once
#include "PrimitiveComponent.h"
#include "UTexture.h"

struct FTexture;

struct FBillboardComponentInfo : public FPrimitiveComponentInfo
{
    DECLARE_ACTORCOMPONENT_INFO(FBillboardComponentInfo);
    FWString TexturePath; 

    FBillboardComponentInfo()
        : FPrimitiveComponentInfo()
        , TexturePath(L"")
    {
        InfoType = TEXT("FBillboardComponentInfo");
        ComponentClass = TEXT("UBillboardComponent");
    }
    
    virtual void Copy(FActorComponentInfo& Other) override
    {
        FPrimitiveComponentInfo::Copy(Other);
        FBillboardComponentInfo& OtherBillboard = static_cast<FBillboardComponentInfo&>(Other);
        OtherBillboard.TexturePath = TexturePath;
    }

    virtual void Serialize(FArchive& ar) const override
    {
        FPrimitiveComponentInfo::Serialize(ar);
        ar << TexturePath;
    }

    virtual void Deserialize(FArchive& ar) override
    {
        FPrimitiveComponentInfo::Deserialize(ar);
        ar >> TexturePath;
    }
};

class UBillboardComponent : public UPrimitiveComponent
{
    DECLARE_CLASS(UBillboardComponent, UPrimitiveComponent)
    
public:
    UBillboardComponent();
    virtual ~UBillboardComponent() override;
    UBillboardComponent(const UBillboardComponent& other);

    virtual void InitializeComponent() override;
    virtual void TickComponent(float DeltaTime) override;
    virtual int CheckRayIntersection(
        FVector& rayOrigin,
        FVector& rayDirection, float& pfNearHitDistance
    ) override;

    void SetTexture(FWString _fileName);
    //void SetUUIDParent(USceneComponent* _parent);
    FMatrix CreateBillboardMatrix();
    virtual UObject* Duplicate() const override;
    virtual void DuplicateSubObjects(const UObject* Source) override;
    virtual void PostDuplicate() override;
    
    float finalIndexU = 0.0f;
    float finalIndexV = 0.0f;
    std::shared_ptr<FTexture> Texture;

public:
    std::unique_ptr<FActorComponentInfo> GetComponentInfo() override;
    virtual void SaveComponentInfo(FActorComponentInfo& OutInfo) override;
    void LoadAndConstruct(const FActorComponentInfo& Info) override;

protected:
    //USceneComponent* m_parent = nullptr;

    bool CheckPickingOnNDC(const TArray<FVector>& checkQuad, float& hitDistance);

private:
    void CreateQuadTextureVertexBuffer();
};