#include "UParticleSubUVComp.h"

#include "LaunchEngineLoop.h"
#include "Renderer/Renderer.h"
#include "CoreUObject/UObject/ObjectFactory.h"
#include "CoreUObject/UObject/Casts.h"


UParticleSubUVComp::UParticleSubUVComp()
{
    bIsLoop = true;
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

UObject* UParticleSubUVComp::Duplicate(UObject* InOuter)
{
    UParticleSubUVComp* Cloned = Cast<ThisClass>(Super::Duplicate(InOuter));
    Cloned->DuplicateSubObjects(this, InOuter);
    return Cloned;
}

void UParticleSubUVComp::DuplicateSubObjects(const UObject* Source, UObject* InOuter)
{
    UBillboardComponent::DuplicateSubObjects(Source, InOuter);
}

void UParticleSubUVComp::SetRowColumnCount(int _cellsPerRow, int _cellsPerColumn)
{
	CellsPerRow = _cellsPerRow;
	CellsPerColumn = _cellsPerColumn;

	CreateSubUVVertexBuffer();
}

std::unique_ptr<FActorComponentInfo> UParticleSubUVComp::GetComponentInfo()
{
    auto Info = std::make_unique<FParticleSubUVCompInfo>();
    SaveComponentInfo(*Info);
    
    return Info;
}

void UParticleSubUVComp::SaveComponentInfo(FActorComponentInfo& OutInfo)
{

    FParticleSubUVCompInfo* Info = static_cast<FParticleSubUVCompInfo*>(&OutInfo);
    Super::SaveComponentInfo(*Info);

    Info->CellsPerRow = CellsPerRow;
    Info->CellsPerColumn = CellsPerColumn;

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

	ID3D11Buffer* VB = GEngineLoop.Renderer.GetResourceManager()->CreateImmutableVertexBuffer<FVertexTexture>(vertices);
    GEngineLoop.Renderer.GetResourceManager()->AddOrSetVertexBuffer(TEXT("QuadVB"), VB);
    GEngineLoop.Renderer.MappingVBTopology(TEXT("Quad"), TEXT("QuadVB"), sizeof(FVertexTexture), 4);

    VBIBTopologyMappingName = TEXT("Quad");
}
