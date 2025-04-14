#include "Engine/Source/Editor/PropertyEditor/ShowFlags.h"
#include "UParticleSubUVComp.h"

#include "EditorEngine.h"
#include "UnrealEd/EditorViewportClient.h"
#include "Engine/World.h"
#include "LevelEditor/SLevelEditor.h"


UParticleSubUVComp::UParticleSubUVComp()
{
    bIsLoop = true;
}

UParticleSubUVComp::UParticleSubUVComp(const UParticleSubUVComp& other)
    : UBillboardComponent(other)
    , bIsLoop(other.bIsLoop)
    , indexU(other.indexU)
    , indexV(other.indexV)
    , second(other.second)
    , CellsPerColumn(other.CellsPerColumn)
    , CellsPerRow(other.CellsPerRow)
{
}

UParticleSubUVComp::~UParticleSubUVComp()
{

}

void UParticleSubUVComp::InitializeComponent()
{
	Super::InitializeComponent();
	// UEditorEngine::renderer.GetConstantBufferUpdater().UpdateSubUVConstant(UEditorEngine::renderer.SubUVConstantBuffer, 0, 0);
	// UEditorEngine::renderer.PrepareSubUVConstant();
}

void UParticleSubUVComp::TickComponent(float DeltaTime)
{
    Super::TickComponent(DeltaTime);
    if (!IsActive()) return;

	uint32 CellWidth = Texture->width / CellsPerColumn;
	uint32 CellHeight = Texture->height / CellsPerColumn;


	second += DeltaTime;
	if (second >= 75)
	{
		indexU++;
		second = 0;
	}
	if (indexU >= CellsPerColumn)
	{
		indexU = 0;
		indexV++;
	}
	if (indexV >= CellsPerRow)
	{
		indexU = 0;
		indexV = 0;

	    // TODO: 파티클 제거는 따로 안하고, Actor에 LifeTime을 설정하든가, 파티클의 Activate 설정을 추가하던가 하기로
	    if (!bIsLoop)
	    {
            Deactivate();
	    }
	    // DestroyComponent();
		// GetWorld()->ThrowAwayObj(this);
		// GetWorld()->SetPickingObj(nullptr);
	}


	float normalWidthOffset = float(CellWidth) / float(Texture->width);
	float normalHeightOffset = float(CellHeight) / float(Texture->height);

	finalIndexU = float(indexU) * normalWidthOffset;
	finalIndexV = float(indexV) * normalHeightOffset;
}

UObject* UParticleSubUVComp::Duplicate() const
{
    UParticleSubUVComp* Cloned = FObjectFactory::ConstructObjectFrom(this);
    Cloned->DuplicateSubObjects(this);
    return Cloned;
}

void UParticleSubUVComp::DuplicateSubObjects(const UObject* Source)
{
    UBillboardComponent::DuplicateSubObjects(Source);
}

void UParticleSubUVComp::SetRowColumnCount(int _cellsPerRow, int _cellsPerColumn)
{
	CellsPerRow = _cellsPerRow;
	CellsPerColumn = _cellsPerColumn;

	CreateSubUVVertexBuffer();
}

std::shared_ptr<FActorComponentInfo> UParticleSubUVComp::GetActorComponentInfo()
{
    std::shared_ptr<FParticleSubUVCompInfo> Info = std::make_shared<FParticleSubUVCompInfo>();
    Super::GetActorComponentInfo()->Copy(*Info);

    Info->CellsPerRow = CellsPerRow;
    Info->CellsPerColumn = CellsPerColumn;

    return Info;
}

void UParticleSubUVComp::LoadAndConstruct(const FActorComponentInfo& Info)
{
    Super::LoadAndConstruct(Info);
    const FParticleSubUVCompInfo& SubUVInfo = static_cast<const FParticleSubUVCompInfo&>(Info);
    CellsPerRow = SubUVInfo.CellsPerRow;
    CellsPerColumn = SubUVInfo.CellsPerColumn;
}

void UParticleSubUVComp::UpdateVertexBuffer(const TArray<FVertexTexture>& vertices)
{

}

void UParticleSubUVComp::CreateSubUVVertexBuffer()
{

	uint32 CellWidth = Texture->width/CellsPerColumn;
	uint32 CellHeight = Texture->height/ CellsPerColumn;
	float normalWidthOffset = float(CellWidth) / float(Texture->width);
	float normalHeightOffset = float(CellHeight) / float(Texture->height);

	TArray<FVertexTexture> vertices =
	{
		{-1.0f,1.0f,0.0f,0,0},
		{ 1.0f,1.0f,0.0f,1,0},
		{-1.0f,-1.0f,0.0f,0,1},
		{ 1.0f,-1.0f,0.0f,1,1}
	};
	vertices[1].u = normalWidthOffset;
	vertices[2].v = normalHeightOffset;
	vertices[3].u = normalWidthOffset;
	vertices[3].v = normalHeightOffset;

	ID3D11Buffer* VB = UEditorEngine::renderer.GetResourceManager()->CreateImmutableVertexBuffer<FVertexTexture>(vertices);
    UEditorEngine::renderer.GetResourceManager()->AddOrSetVertexBuffer(TEXT("QuadVB"), VB);
    UEditorEngine::renderer.MappingVBTopology(TEXT("Quad"), TEXT("QuadVB"), sizeof(FVertexTexture), 4);

    VBIBTopologyMappingName = TEXT("Quad");
}
