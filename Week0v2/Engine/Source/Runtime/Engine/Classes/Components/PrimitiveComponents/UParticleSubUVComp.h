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
        ComponentClass = TEXT("UParticleSubUVComp");
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
    UParticleSubUVComp(const UParticleSubUVComp& other);
    virtual ~UParticleSubUVComp() override;

    virtual void InitializeComponent() override;
    virtual void TickComponent(float DeltaTime) override;

    UObject* Duplicate() const override;
    void DuplicateSubObjects(const UObject* Source) override;
    void SetRowColumnCount(int _cellsPerRow, int _cellsPerColumn);

    // ID3D11Buffer* vertexSubUVBuffer;
    //UINT numTextVertices;

public:
    std::unique_ptr<FActorComponentInfo> GetComponentInfo() override;
    virtual void SaveComponentInfo(FActorComponentInfo& OutInfo) override;
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
