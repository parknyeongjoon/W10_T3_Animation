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
        ComponentType = TEXT("UText");
    }
    virtual void Copy(FActorComponentInfo& Other) override
    {
        FBillboardComponentInfo::Copy(Other);
        FTextComponentInfo& TextInfo = static_cast<FTextComponentInfo&>(Other);
        TextInfo.Text = Text;
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

class UText : public UBillboardComponent
{
    DECLARE_CLASS(UText, UBillboardComponent)

public:
    UText();
    virtual ~UText() override;
    UText(const UText& other);
    virtual void InitializeComponent() override;
    virtual void TickComponent(float DeltaTime) override;
    void ClearText();
    void SetText(const FWString& _text);
    FWString GetText() { return text; }
    void SetRowColumnCount(int _cellsPerRow, int _cellsPerColumn);
    virtual int CheckRayIntersection(FVector& rayOrigin, FVector& rayDirection, float& pfNearHitDistance) override;
    virtual UObject* Duplicate() const override;
    virtual void DuplicateSubObjects(const UObject* Source) override;
    virtual void PostDuplicate() override;

    TArray<FVertexTexture> vertexTextureArr;

public:    
    virtual std::shared_ptr<FActorComponentInfo> GetActorComponentInfo() override;
    virtual void LoadAndConstruct(const FActorComponentInfo& Info);

protected:

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
