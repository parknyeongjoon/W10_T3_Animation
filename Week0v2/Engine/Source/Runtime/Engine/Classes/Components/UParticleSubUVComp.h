#pragma once
#include "UBillboardComponent.h"

struct FParticleSubUVCompInfo : public FBillboardComponentInfo
{
    DECLARE_ACTORCOMPONENT_INFO(FParticleSubUVCompInfo);
    int CellsPerRow;
    int CellsPerColumn;

    FParticleSubUVCompInfo()
        : FBillboardComponentInfo()
        , CellsPerRow(1)
        , CellsPerColumn(1)
    {
        InfoType = TEXT("FParticleSubUVCompInfo");
        ComponentType = TEXT("UParticleSubUVComp");
    }

    virtual void Copy(FActorComponentInfo& Other) override
    {
        FBillboardComponentInfo::Copy(Other);
        FParticleSubUVCompInfo& OtherSubUV = static_cast<FParticleSubUVCompInfo&>(Other);
        OtherSubUV.CellsPerRow = CellsPerRow;
        OtherSubUV.CellsPerColumn = CellsPerColumn;
    }

    virtual void Serialize(FArchive& ar) const override
    {
        FBillboardComponentInfo::Serialize(ar);
        ar << CellsPerRow << CellsPerColumn;
    }

    virtual void Deserialize(FArchive& ar) override
    {
        FBillboardComponentInfo::Deserialize(ar);
        ar >> CellsPerRow >> CellsPerColumn;
    }
};

class UParticleSubUVComp : public UBillboardComponent
{
    DECLARE_CLASS(UParticleSubUVComp, UBillboardComponent)

public:
    UParticleSubUVComp();
    virtual ~UParticleSubUVComp() override;

    virtual void InitializeComponent() override;
    virtual void TickComponent(float DeltaTime) override;

    void SetRowColumnCount(int _cellsPerRow, int _cellsPerColumn);

    ID3D11Buffer* vertexSubUVBuffer;
    UINT numTextVertices;

public:
    virtual std::shared_ptr<FActorComponentInfo> GetActorComponentInfo();
    virtual void LoadAndConstruct(const FActorComponentInfo& Info) override;

protected:
    bool bIsLoop = true;

private:
    int indexU = 0;
    int indexV = 0;
    float second = 0;

    int CellsPerRow;
    int CellsPerColumn;

    void UpdateVertexBuffer(const TArray<FVertexTexture>& vertices);
    void CreateSubUVVertexBuffer();
};
