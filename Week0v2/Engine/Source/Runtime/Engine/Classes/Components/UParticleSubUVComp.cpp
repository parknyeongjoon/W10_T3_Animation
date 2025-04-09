#include "Engine/Source/Editor/PropertyEditor/ShowFlags.h"
#include "UParticleSubUVComp.h"
#include "UnrealEd/EditorViewportClient.h"
#include "Engine/World.h"
#include "LevelEditor/SLevelEditor.h"


UParticleSubUVComp::UParticleSubUVComp()
{
    bIsLoop = true;
}

UParticleSubUVComp::~UParticleSubUVComp()
{
	if (vertexSubUVBuffer)
	{
		vertexSubUVBuffer->Release();
		vertexSubUVBuffer = nullptr;
	}
}

void UParticleSubUVComp::InitializeComponent()
{
	Super::InitializeComponent();
	UEditorEngine::renderer.GetConstantBufferUpdater().UpdateSubUVConstant(UEditorEngine::renderer.SubUVConstantBuffer, 0, 0);
	UEditorEngine::renderer.PrepareSubUVConstant();
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
	/*
	ID3D11DeviceContext* context = FEngineLoop::graphicDevice.DeviceContext;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	context->Map(vertexTextureBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, vertices.GetData(), vertices.Num() * sizeof(FVertexTexture));
	context->Unmap(vertexTextureBuffer, 0);
	*/

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

	vertexSubUVBuffer = UEditorEngine::renderer.GetResourceManager().CreateVertexBuffer(vertices);
	numTextVertices = static_cast<UINT>(vertices.Num());
}
