#pragma once
#include "UBillboardComponent.h"

struct FTextComponentInfo : public FBillboardComponentInfo
{
    DECLARE_ACTORCOMPONENT_INFO(FTextComponentInfo);

    FWString Text;

    FTextComponentInfo()
        : FBillboardComponentInfo()
        , Text(L"")
    {
        InfoType = TEXT("FTextComponentInfo");
        ComponentClass = TEXT("UText");
    }

    virtual void Serialize(FArchive& ar) const override
    {
        FBillboardComponentInfo::Serialize(ar);
        ar << Text;
    }

    virtual void Deserialize(FArchive& ar) override
    {
        FBillboardComponentInfo::Deserialize(ar);
        ar >> Text;
    }
};

class UTextComponent : public UBillboardComponent
{
    DECLARE_CLASS(UTextComponent, UBillboardComponent)

public:
    UTextComponent();
    virtual ~UTextComponent() override;
    virtual void InitializeComponent() override;
    virtual void TickComponent(float DeltaTime) override;
    void ClearText();
    void SetText(const FWString& _text);
    FWString GetText() { return text; }
    void SetRowColumnCount(int _cellsPerRow, int _cellsPerColumn);
    virtual int CheckRayIntersection(FVector& rayOrigin, FVector& rayDirection, float& pfNearHitDistance) override;
    virtual UObject* Duplicate(UObject* InOuter) override;
    virtual void DuplicateSubObjects(const UObject* Source, UObject* InOuter) override;
    virtual void PostDuplicate() override;

    TArray<FVertexTexture> vertexTextureArr;

public:    
    virtual void SaveComponentInfo(FActorComponentInfo& OutInfo) override;
    
    std::unique_ptr<FActorComponentInfo> GetComponentInfo() override;

protected:
    virtual void LoadAndConstruct(const FActorComponentInfo& Info) override;

    FWString text;

    TArray<FVector> quad;

    const int quadSize = 2;

    int RowCount;
    int ColumnCount;

    float quadWidth = 2.0f;
    float quadHeight = 2.0f;

    void setStartUV(char alphabet, float& outStartU, float& outStartV) const;
    void setStartUV(wchar_t hangul, float& outStartU, float& outStartV) const;

    void TextMVPRendering();
};
