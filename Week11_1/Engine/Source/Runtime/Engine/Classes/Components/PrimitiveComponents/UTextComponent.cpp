#include "UTextComponent.h"

#include "EditorEngine.h"
#include "LaunchEngineLoop.h"
#include "ShowFlags.h"
#include "Engine/Texture.h"
#include "Engine/World.h"
#include "LevelEditor/SLevelEditor.h"
#include "Renderer/Renderer.h"
#include "UnrealEd/EditorViewportClient.h"
#include "UObject/Casts.h"

extern UEngine* GEngine;

UTextComponent::UTextComponent()
{
}

UTextComponent::~UTextComponent()
{

}

void UTextComponent::InitializeComponent()
{
    Super::InitializeComponent();
}

void UTextComponent::TickComponent(float DeltaTime)
{
	Super::TickComponent(DeltaTime);
    
}

void UTextComponent::ClearText()
{
    vertexTextureArr.Empty();
}
void UTextComponent::SetRowColumnCount(int _cellsPerRow, int _cellsPerColumn) 
{
    RowCount = _cellsPerRow;
    ColumnCount = _cellsPerColumn;
}

int UTextComponent::CheckRayIntersection(FVector& rayOrigin, FVector& rayDirection, float& pfNearHitDistance)
{
    UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
    if (EditorEngine == nullptr)
    {
        return 0;
    }

    SLevelEditor* LevelEditor = EditorEngine->GetLevelEditor();
    if (LevelEditor == nullptr)
    {
        return 0;
    }
    
	if (!(LevelEditor->GetActiveViewportClient()->ShowFlag & static_cast<uint64>(EEngineShowFlags::SF_BillboardText))) {
		return 0;
	}
	for (int i = 0; i < vertexTextureArr.Num(); i++)
	{
		quad.Add(FVector(vertexTextureArr[i].x,
			vertexTextureArr[i].y, vertexTextureArr[i].z));
	}

	return CheckPickingOnNDC(quad,pfNearHitDistance);
}

UObject* UTextComponent::Duplicate(UObject* InOuter)
{
    UTextComponent* ClonedActor = Cast<ThisClass>(Super::Duplicate(InOuter));
    ClonedActor->DuplicateSubObjects(this, InOuter);
    ClonedActor->PostDuplicate();
    return ClonedActor;
}

void UTextComponent::DuplicateSubObjects(const UObject* Source, UObject* InOuter)
{
    UBillboardComponent::DuplicateSubObjects(Source, InOuter);
}

void UTextComponent::PostDuplicate()
{
    UBillboardComponent::PostDuplicate();
}

std::unique_ptr<FActorComponentInfo> UTextComponent::GetComponentInfo()
{
    auto Info = std::make_unique<FTextComponentInfo>();
    SaveComponentInfo(*Info);
    
    return Info;
}


void UTextComponent::SaveComponentInfo(FActorComponentInfo& OutInfo)
{
    FTextComponentInfo* Info = static_cast<FTextComponentInfo*>(&OutInfo);
    Super::SaveComponentInfo(*Info);
    
    Info->Text = text;
}

void UTextComponent::LoadAndConstruct(const FActorComponentInfo& Info)
{
    Super::LoadAndConstruct(Info);

    // cast
    const FTextComponentInfo& TextInfo = static_cast<const FTextComponentInfo&>(Info);
    SetText(TextInfo.Text);
}


void UTextComponent::SetText(const FWString& _text)
{
	text = _text;
	if (_text.empty())
	{
		Console::GetInstance().AddLog(LogLevel::Warning, "Text is empty");

		vertexTextureArr.Empty();
		quad.Empty();
	    
		return;
	}
	int textSize = static_cast<int>(_text.size());


	const uint32 BitmapWidth = Texture->width;
	const uint32 BitmapHeight = Texture->height;

	const float CellWidth =  static_cast<float>(BitmapWidth) / ColumnCount;
	const float CellHeight = static_cast<float>(BitmapHeight) / RowCount;

	const float nTexelUOffset = CellWidth / BitmapWidth;
	const float nTexelVOffset = CellHeight/ BitmapHeight;

	for (int i = 0; i < _text.size(); i++)
	{
		FVertexTexture leftUP = { -1.0f,1.0f,0.0f,0.0f,0.0f };
		FVertexTexture rightUP = { 1.0f,1.0f,0.0f,1.0f,0.0f };
		FVertexTexture leftDown = { -1.0f,-1.0f,0.0f,0.0f,1.0f };
		FVertexTexture rightDown = { 1.0f,-1.0f,0.0f,1.0f,1.0f };
		rightUP.u *= nTexelUOffset;
		leftDown.v *= nTexelVOffset;
		rightDown.u *= nTexelUOffset;
		rightDown.v *= nTexelVOffset;

		leftUP.x += quadWidth * i;
		rightUP.x += quadWidth * i;
		leftDown.x += quadWidth * i;
		rightDown.x += quadWidth * i;

		float startU = 0.0f;
		float startV = 0.0f;

		setStartUV(_text[i], startU, startV);
		leftUP.u += (nTexelUOffset * startU);
		leftUP.v += (nTexelVOffset * startV);
		rightUP.u += (nTexelUOffset * startU);
		rightUP.v += (nTexelVOffset * startV);
		leftDown.u += (nTexelUOffset * startU);
		leftDown.v += (nTexelVOffset * startV);
		rightDown.u += (nTexelUOffset * startU);
		rightDown.v += (nTexelVOffset * startV);

		vertexTextureArr.Add(leftUP);
		vertexTextureArr.Add(rightUP);
		vertexTextureArr.Add(leftDown);
		vertexTextureArr.Add(rightUP);
		vertexTextureArr.Add(rightDown);
		vertexTextureArr.Add(leftDown);
	}

	const float lastX = -1.0f + quadSize * _text.size();
	quad.Add(FVector(-1.0f,1.0f,0.0f));
	quad.Add(FVector(-1.0f,-1.0f,0.0f));
	quad.Add(FVector(lastX,1.0f,0.0f));
	quad.Add(FVector(lastX,-1.0f,0.0f));

	//CreateTextTextureVertexBuffer(vertexTextureArr,byteWidth);
    ID3D11Buffer* VB = nullptr;
    
    VB = GEngineLoop.Renderer.GetResourceManager()->CreateImmutableVertexBuffer<FVertexTexture>(vertexTextureArr);

    int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, 
                                     text.c_str(), -1, 
                                     nullptr, 0, 
                                     nullptr, nullptr);
    std::string result(sizeNeeded, 0);
    WideCharToMultiByte(CP_UTF8, 0, 
                        text.c_str(), -1, 
                        &result[0], sizeNeeded, 
                        nullptr, nullptr);

    FString textName = result;

    result.pop_back(); // 널 문자 제거
    GEngineLoop.Renderer.GetResourceManager()->AddOrSetVertexBuffer(textName, VB);
    GEngineLoop.Renderer.MappingVBTopology(textName, textName, sizeof(FVertexSimple), vertexTextureArr.Num());
}

void UTextComponent::setStartUV(const wchar_t hangul, float& outStartU, float& outStartV) const
{
    //대문자만 받는중
    int StartU = 0;
    int StartV = 0;
    int offset = -1;

    if (hangul == L' ') {
        outStartU = 0;  // Space는 특별히 UV 좌표를 (0,0)으로 설정
        outStartV = 0;
        offset = 0;
        return;
    }
    else if (hangul >= L'A' && hangul <= L'Z') {

        StartU = 11;
        StartV = 0;
        offset = hangul - L'A'; // 대문자 위치
    }
    else if (hangul >= L'a' && hangul <= L'z') {
        StartU = 37;
        StartV = 0;
        offset = (hangul - L'a'); // 소문자는 대문자 다음 위치
    }
    else if (hangul >= L'0' && hangul <= L'9') {
        StartU = 1;
        StartV = 0;
        offset = (hangul - L'0'); // 숫자는 소문자 다음 위치
    }
    else if (hangul >= L'가' && hangul <= L'힣')
    {
        StartU = 63;
        StartV = 0;
        offset = hangul - L'가'; // 대문자 위치
    }

    if (offset == -1)
    {
        Console::GetInstance().AddLog(LogLevel::Warning, "Text Error");
    }

    int offsetV = (offset + StartU) / ColumnCount;
    int offsetU = (offset + StartU) % ColumnCount;

    outStartU = static_cast<float>(offsetU);
    outStartV = static_cast<float>(StartV + offsetV);
}

void UTextComponent::setStartUV(const char alphabet, float& outStartU, float& outStartV) const
{
    //대문자만 받는중
    int StartU=0;
    int StartV=0;
    int offset = -1;


    if (alphabet == ' ') {
        outStartU = 0;  // Space는 특별히 UV 좌표를 (0,0)으로 설정
        outStartV = 0;
        offset = 0;
        return;
    }
    else if (alphabet >= 'A' && alphabet <= 'Z') {

        StartU = 1;
        StartV = 4;
        offset = alphabet - 'A'; // 대문자 위치
    }
    else if (alphabet >= 'a' && alphabet <= 'z') {
        StartU = 1;
        StartV = 6;
        offset = (alphabet - 'a'); // 소문자는 대문자 다음 위치
    }
    else if (alphabet >= '0' && alphabet <= '9') {
        StartU = 0;
        StartV = 3;
        offset = (alphabet - '0'); // 숫자는 소문자 다음 위치
    }

    if (offset == -1)
    {
        Console::GetInstance().AddLog(LogLevel::Warning, "Text Error");
    }

    int offsetV = (offset + StartU) / ColumnCount;
    int offsetU = (offset + StartU) % ColumnCount;

    outStartU = static_cast<float>(offsetU);
    outStartV = static_cast<float>(StartV + offsetV);

}


void UTextComponent::TextMVPRendering()
{
    // UEditorEngine::renderer.PrepareTextureShader();
    // //FEngineLoop::renderer.UpdateSubUVConstant(0, 0);
    // //FEngineLoop::renderer.PrepareSubUVConstant();
    // FMatrix Model = CreateBillboardMatrix();
    //
    // FMatrix ViewProj = GetEngine()->GetLevelEditor()->GetActiveViewportClient()->GetViewMatrix() * GetEngine()->GetLevelEditor()->GetActiveViewportClient()->GetProjectionMatrix();
    //
    // FMatrix NormalMatrix = FMatrix::Transpose(FMatrix::Inverse(Model));
    // FVector4 UUIDColor = EncodeUUID() / 255.0f;
    // if (this == GetWorld()->GetPickingGizmo()) {
    //     UEditorEngine::renderer.GetConstantBufferUpdater().UpdateConstant(UEditorEngine::renderer.ConstantBuffer, Model, ViewProj, NormalMatrix, UUIDColor, true);
    // }
    // else
    //     UEditorEngine::renderer.GetConstantBufferUpdater().UpdateConstant(UEditorEngine::renderer.ConstantBuffer, Model, ViewProj, NormalMatrix, UUIDColor, false);
    //
    // if (ShowFlags::GetInstance().currentFlags & static_cast<uint64>(EEngineShowFlags::SF_BillboardText)) {
    //     UEditorEngine::renderer.RenderTextPrimitive(vertexTextBuffer, numTextVertices,
    //         Texture->TextureSRV, Texture->SamplerState);
    // }
    // //Super::Render();
    //
    // UEditorEngine::renderer.PrepareShader();
}
