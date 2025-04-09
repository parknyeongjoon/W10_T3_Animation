#include "UText.h"

#include "Engine/World.h"
#include "Engine/Source/Editor/PropertyEditor/ShowFlags.h"
#include "UnrealEd/EditorViewportClient.h"
#include "LevelEditor/SLevelEditor.h"

UText::UText()
{
}

UText::~UText()
{
	if (vertexTextBuffer)
	{
		vertexTextBuffer->Release();
		vertexTextBuffer = nullptr;
	}
}

UText::UText(const UText& other) :UBillboardComponent(other), vertexTextBuffer(other.vertexTextBuffer), vertexTextureArr(other.vertexTextureArr)
,numTextVertices(other.numTextVertices), text(other.text), quad(other.quad)
{
}

void UText::InitializeComponent()
{
    Super::InitializeComponent();
}

void UText::TickComponent(float DeltaTime)
{
	Super::TickComponent(DeltaTime);
    
}

void UText::ClearText()
{
    vertexTextureArr.Empty();
}
void UText::SetRowColumnCount(int _cellsPerRow, int _cellsPerColumn) 
{
    RowCount = _cellsPerRow;
    ColumnCount = _cellsPerColumn;
}

int UText::CheckRayIntersection(FVector& rayOrigin, FVector& rayDirection, float& pfNearHitDistance)
{
	if (!(ShowFlags::GetInstance().currentFlags & static_cast<uint64>(EEngineShowFlags::SF_BillboardText))) {
		return 0;
	}
	for (int i = 0; i < vertexTextureArr.Num(); i++)
	{
		quad.Add(FVector(vertexTextureArr[i].x,
			vertexTextureArr[i].y, vertexTextureArr[i].z));
	}

	return CheckPickingOnNDC(quad,pfNearHitDistance);
}

UObject* UText::Duplicate() const
{
    UText* ClonedActor = FObjectFactory::ConstructObjectFrom<UText>(this);
    ClonedActor->DuplicateSubObjects(this);
    ClonedActor->PostDuplicate();
    return ClonedActor;
}

void UText::DuplicateSubObjects(const UObject* Source)
{
    UBillboardComponent::DuplicateSubObjects(Source);
}

void UText::PostDuplicate()
{
    UBillboardComponent::PostDuplicate();
}

std::shared_ptr<FActorComponentInfo> UText::GetActorComponentInfo()
{
    std::shared_ptr<FTextComponentInfo> Info = std::make_shared<FTextComponentInfo>();
    Super::GetActorComponentInfo()->Copy(*Info);

    Info->Text = text;

    return Info;
}

void UText::LoadAndConstruct(const FActorComponentInfo& Info)
{
    Super::LoadAndConstruct(Info);

    // cast
    const FTextComponentInfo& TextInfo = static_cast<const FTextComponentInfo&>(Info);
    SetText(TextInfo.Text);
}


void UText::SetText(FWString _text)
{
	text = _text;
	if (_text.empty())
	{
		Console::GetInstance().AddLog(LogLevel::Warning, "Text is empty");

		vertexTextureArr.Empty();
		quad.Empty();

		// 기존 버텍스 버퍼가 있다면 해제
		if (vertexTextBuffer)
		{
			vertexTextBuffer->Release();
			vertexTextBuffer = nullptr;
		}
		return;
	}
	int textSize = static_cast<int>(_text.size());


	uint32 BitmapWidth = Texture->width;
	uint32 BitmapHeight = Texture->height;

	float CellWidth =  float(BitmapWidth)/ColumnCount;
	float CellHeight = float(BitmapHeight)/RowCount;

	float nTexelUOffset = CellWidth / BitmapWidth;
	float nTexelVOffset = CellHeight/ BitmapHeight;

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
	UINT byteWidth = static_cast<UINT>(vertexTextureArr.Num() * sizeof(FVertexTexture));

	float lastX = -1.0f + quadSize* _text.size();
	quad.Add(FVector(-1.0f,1.0f,0.0f));
	quad.Add(FVector(-1.0f,-1.0f,0.0f));
	quad.Add(FVector(lastX,1.0f,0.0f));
	quad.Add(FVector(lastX,-1.0f,0.0f));

	CreateTextTextureVertexBuffer(vertexTextureArr,byteWidth);
}
void UText::setStartUV(wchar_t hangul, float& outStartU, float& outStartV)
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
void UText::setStartUV(char alphabet, float& outStartU, float& outStartV)
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
void UText::CreateTextTextureVertexBuffer(const TArray<FVertexTexture>& _vertex,UINT byteWidth)
{
	numTextVertices = static_cast<UINT>(_vertex.Num());
	// 2. Create a vertex buffer
	D3D11_BUFFER_DESC vertexbufferdesc = {};
	vertexbufferdesc.ByteWidth = byteWidth;
	vertexbufferdesc.Usage = D3D11_USAGE_IMMUTABLE; // will never be updated 
	vertexbufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vertexbufferSRD = { _vertex.GetData()};

	ID3D11Buffer* vertexBuffer;
	
	HRESULT hr = UEditorEngine::graphicDevice.Device->CreateBuffer(&vertexbufferdesc, &vertexbufferSRD, &vertexBuffer);
	if (FAILED(hr))
	{
		UE_LOG(LogLevel::Warning, "VertexBuffer Creation faild");
	}
	vertexTextBuffer = vertexBuffer;

	//FEngineLoop::resourceMgr.RegisterMesh(&FEngineLoop::renderer, "JungleText", _vertex, _vertex.Num() * sizeof(FVertexTexture),
	//	nullptr, 0);

}


void UText::TextMVPRendering()
{
    UEditorEngine::renderer.PrepareTextureShader();
    //FEngineLoop::renderer.UpdateSubUVConstant(0, 0);
    //FEngineLoop::renderer.PrepareSubUVConstant();
    FMatrix Model = CreateBillboardMatrix();

    FMatrix ViewProj = GetEngine()->GetLevelEditor()->GetActiveViewportClient()->GetViewMatrix() * GetEngine()->GetLevelEditor()->GetActiveViewportClient()->GetProjectionMatrix();

    FMatrix NormalMatrix = FMatrix::Transpose(FMatrix::Inverse(Model));
    FVector4 UUIDColor = EncodeUUID() / 255.0f;
    if (this == GetWorld()->GetPickingGizmo()) {
        UEditorEngine::renderer.GetConstantBufferUpdater().UpdateConstant(UEditorEngine::renderer.ConstantBuffer, Model, ViewProj, NormalMatrix, UUIDColor, true);
    }
    else
        UEditorEngine::renderer.GetConstantBufferUpdater().UpdateConstant(UEditorEngine::renderer.ConstantBuffer, Model, ViewProj, NormalMatrix, UUIDColor, false);

    if (ShowFlags::GetInstance().currentFlags & static_cast<uint64>(EEngineShowFlags::SF_BillboardText)) {
        UEditorEngine::renderer.RenderTextPrimitive(vertexTextBuffer, numTextVertices,
            Texture->TextureSRV, Texture->SamplerState);
    }
    //Super::Render();

    UEditorEngine::renderer.PrepareShader();
}
