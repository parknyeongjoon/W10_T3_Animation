#include "UBillboardComponent.h"
#include "Actors/Player.h"
#include "QuadTexture.h"
#include "Define.h"
#include <DirectXMath.h>

#include "Engine/World.h"
#include "Math/MathUtility.h"
#include "UnrealEd/EditorViewportClient.h"
#include "LevelEditor/SLevelEditor.h"
#include "PropertyEditor/ShowFlags.h"


UBillboardComponent::UBillboardComponent()
{
}

UBillboardComponent::~UBillboardComponent()
{
	if (vertexTextureBuffer)
	{
		vertexTextureBuffer->Release();
		vertexTextureBuffer = nullptr;
	}
	if (indexTextureBuffer)
	{
		indexTextureBuffer->Release();
		indexTextureBuffer = nullptr;
	}
}

UBillboardComponent::UBillboardComponent(const UBillboardComponent& other) : UPrimitiveComponent(other),
vertexTextureBuffer(other.vertexTextureBuffer),indexTextureBuffer(other.indexTextureBuffer), numIndices(other.numIndices),
numVertices(other.numVertices), finalIndexU(other.finalIndexU), finalIndexV(other.finalIndexV),Texture(other.Texture)
{
}

void UBillboardComponent::InitializeComponent()
{
    // Super::InitializeComponent();
	CreateQuadTextureVertexBuffer();
}



void UBillboardComponent::TickComponent(float DeltaTime)
{
    Super::TickComponent(DeltaTime);
}


int UBillboardComponent::CheckRayIntersection(FVector& rayOrigin, FVector& rayDirection, float& pfNearHitDistance)
{
	TArray<FVector> quad;
	for (int i = 0; i < 4; i++)
	{
		quad.Add(FVector(quadTextureVertices[i].x, 
			quadTextureVertices[i].y, quadTextureVertices[i].z));
	}
	return CheckPickingOnNDC(quad,pfNearHitDistance);
}


void UBillboardComponent::SetTexture(FWString _fileName)
{
	Texture = UEditorEngine::resourceMgr.GetTexture(_fileName);
}

void UBillboardComponent::SetUUIDParent(USceneComponent* _parent)
{
	m_parent = _parent;
}


FMatrix UBillboardComponent::CreateBillboardMatrix()
{
	FMatrix CameraView = GetEngine()->GetLevelEditor()->GetActiveViewportClient()->GetViewMatrix();

	CameraView.M[0][3] = 0.0f;
	CameraView.M[1][3] = 0.0f;
	CameraView.M[2][3] = 0.0f;


	CameraView.M[3][0] = 0.0f;
	CameraView.M[3][1] = 0.0f;
	CameraView.M[3][2] = 0.0f;
	CameraView.M[3][3] = 1.0f;


	CameraView.M[0][2] = -CameraView.M[0][2];
	CameraView.M[1][2] = -CameraView.M[1][2];
	CameraView.M[2][2] = -CameraView.M[2][2];
	FMatrix LookAtCamera = FMatrix::Transpose(CameraView);
	
	FVector worldLocation = GetWorldLocation();
	FVector worldScale = RelativeScale3D;
	FMatrix S = FMatrix::CreateScale(worldScale.x, worldScale.y, worldScale.z);
	FMatrix R = LookAtCamera;
	FMatrix T = FMatrix::CreateTranslationMatrix(worldLocation);
	FMatrix M = S * R * T;

	return M;
}

UObject* UBillboardComponent::Duplicate() const
{
    UBillboardComponent* ClonedActor = FObjectFactory::ConstructObjectFrom<UBillboardComponent>(this);
    ClonedActor->DuplicateSubObjects(this);
    ClonedActor->PostDuplicate();
    return ClonedActor;
}

void UBillboardComponent::DuplicateSubObjects(const UObject* Source)
{
    UPrimitiveComponent::DuplicateSubObjects(Source);
}

void UBillboardComponent::PostDuplicate()
{
    UPrimitiveComponent::PostDuplicate();
}

void UBillboardComponent::CreateQuadTextureVertexBuffer()
{
	numVertices = sizeof(quadTextureVertices) / sizeof(FVertexTexture);
	numIndices = sizeof(quadTextureInices) / sizeof(uint32);
    vertexTextureBuffer = UEditorEngine::renderer.GetResourceManager().CreateVertexBuffer(quadTextureVertices, sizeof(quadTextureVertices));
	indexTextureBuffer = UEditorEngine::renderer.GetResourceManager().CreateIndexBuffer(quadTextureInices, sizeof(quadTextureInices));

	if (!vertexTextureBuffer) {
		Console::GetInstance().AddLog(LogLevel::Warning, "Buffer Error");
	}
	if (!indexTextureBuffer) {
		Console::GetInstance().AddLog(LogLevel::Warning, "Buffer Error");
	}
}

std::shared_ptr<FActorComponentInfo> UBillboardComponent::GetActorComponentInfo()
{
    std::shared_ptr<FBillboardComponentInfo>Info = std::make_shared<FBillboardComponentInfo>();
    Super::GetActorComponentInfo()->Copy(*Info);
    Info->TexturePath = Texture->path;

    return Info;
}

void UBillboardComponent::LoadAndConstruct(const FActorComponentInfo& Info)
{
    Super::LoadAndConstruct(Info);
    const FBillboardComponentInfo& billboardInfo = static_cast<const FBillboardComponentInfo&>(Info);
    SetTexture(billboardInfo.TexturePath);
}

bool UBillboardComponent::CheckPickingOnNDC(const TArray<FVector>& checkQuad, float& hitDistance)
{
	bool result = false;
	POINT mousePos;
	GetCursorPos(&mousePos);
	ScreenToClient(GEngine->hWnd, &mousePos);

	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	UEditorEngine::graphicDevice.DeviceContext->RSGetViewports(&numViewports, &viewport);
	float screenWidth = viewport.Width;
	float screenHeight = viewport.Height;

	FVector pickPosition;
	int screenX = mousePos.x;
	int screenY = mousePos.y;
    FMatrix projectionMatrix = GetEngine()->GetLevelEditor()->GetActiveViewportClient()->GetProjectionMatrix();
	pickPosition.x = ((2.0f * screenX / viewport.Width) - 1);
	pickPosition.y = -((2.0f * screenY / viewport.Height) - 1);
	pickPosition.z = 1.0f; // Near Plane

	FMatrix M = CreateBillboardMatrix();
    FMatrix V = GEngine->GetLevelEditor()->GetActiveViewportClient()->GetViewMatrix();;
	FMatrix P = projectionMatrix;
	FMatrix MVP = M * V * P;

	float minX = FLT_MAX;
	float maxX = FLT_MIN;
	float minY = FLT_MAX;
	float maxY = FLT_MIN;
	float avgZ = 0.0f;
	for (int i = 0; i < checkQuad.Num(); i++)
	{
		FVector4 v = FVector4(checkQuad[i].x, checkQuad[i].y, checkQuad[i].z, 1.0f);
		FVector4 clipPos = FMatrix::TransformVector(v, MVP);
		
		if (clipPos.a != 0)	clipPos = clipPos/clipPos.a;

		minX = FMath::Min(minX, clipPos.x);
		maxX = FMath::Max(maxX, clipPos.x);
		minY = FMath::Min(minY, clipPos.y);
		maxY = FMath::Max(maxY, clipPos.y);
		avgZ += clipPos.z;
	}

	avgZ /= checkQuad.Num();

	if (pickPosition.x >= minX && pickPosition.x <= maxX &&
		pickPosition.y >= minY && pickPosition.y <= maxY)
	{
		float A = P.M[2][2];  // Projection Matrix의 A값 (Z 변환 계수)
		float B = P.M[3][2];  // Projection Matrix의 B값 (Z 변환 계수)

		float z_view_pick = (pickPosition.z - B) / A; // 마우스 클릭 View 공간 Z
		float z_view_billboard = (avgZ - B) / A; // Billboard View 공간 Z

		hitDistance = 1000.0f;
		result = true;
	}

	return result;
}
