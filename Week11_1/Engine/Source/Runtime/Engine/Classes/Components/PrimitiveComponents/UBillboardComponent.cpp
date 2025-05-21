#include "UBillboardComponent.h"
#include "QuadTexture.h"
#include "Define.h"
#include <DirectXMath.h>

#include "EditorEngine.h"
#include "LaunchEngineLoop.h"
#include "WindowsCursor.h"
#include "Engine/World.h"
#include "Math/MathUtility.h"
#include "UnrealEd/EditorViewportClient.h"
#include "LevelEditor/SLevelEditor.h"
#include "Renderer/Renderer.h"


UBillboardComponent::UBillboardComponent()
    : Super()
{
}

UBillboardComponent::~UBillboardComponent()
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
	Texture = GEngineLoop.ResourceManager.GetTexture(_fileName);
}

// void UBillboardComponent::SetUUIDParent(USceneComponent* _parent)
// {
// 	m_parent = _parent;
// }


FMatrix UBillboardComponent::CreateBillboardMatrix()
{
    UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
    if (EditorEngine == nullptr)
    {
        return FMatrix::Identity;
    }
    
	FMatrix CameraView = EditorEngine->GetLevelEditor()->GetActiveViewportClient()->GetViewMatrix();

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
	FVector worldScale = RelativeScale;
	FMatrix S = FMatrix::CreateScaleMatrix(worldScale.X, worldScale.Y, worldScale.Z);
	FMatrix R = LookAtCamera;
	FMatrix T = FMatrix::CreateTranslationMatrix(worldLocation);
	FMatrix M = S * R * T;

	return M;
}

UObject* UBillboardComponent::Duplicate(UObject* InOuter)
{
    UBillboardComponent* ClonedActor = Cast<ThisClass>(Super::Duplicate(InOuter));
    ClonedActor->DuplicateSubObjects(this, InOuter);
    ClonedActor->PostDuplicate();
    return ClonedActor;
}

void UBillboardComponent::DuplicateSubObjects(const UObject* Source, UObject* InOuter)
{
    UPrimitiveComponent::DuplicateSubObjects(Source, InOuter);

}

void UBillboardComponent::PostDuplicate()
{
    UPrimitiveComponent::PostDuplicate();
}

void UBillboardComponent::CreateQuadTextureVertexBuffer()
{
    FRenderer& Renderer = GEngineLoop.Renderer;
    FRenderResourceManager* ResourceManager = Renderer.GetResourceManager();
    
    if (ResourceManager->GetVertexBuffer(TEXT("QuadVB")) && ResourceManager->GetVertexBuffer(TEXT("QuadIB")))
    {
        VBIBTopologyMappingName = TEXT("Quad");
        return;
    }
    
    ID3D11Buffer* VB = ResourceManager->CreateImmutableVertexBuffer(quadTextureVertices, sizeof(quadTextureVertices));
    ResourceManager->AddOrSetVertexBuffer(TEXT("QuadVB"), VB);
    Renderer.MappingVBTopology(TEXT("Quad"), TEXT("QuadVB"), sizeof(FVertexTexture), 4);

    ID3D11Buffer* IB = ResourceManager->CreateIndexBuffer(quadTextureInices, sizeof(quadTextureInices) / sizeof(uint32));
    ResourceManager->AddOrSetIndexBuffer(TEXT("QuadIB"), IB);
    Renderer.MappingIB(TEXT("Quad"), TEXT("QuadIB"), sizeof(quadTextureInices) / sizeof(uint32));

    VBIBTopologyMappingName = TEXT("Quad");
}

std::unique_ptr<FActorComponentInfo> UBillboardComponent::GetComponentInfo()
{
    auto Info = std::make_unique<FBillboardComponentInfo>();
    SaveComponentInfo(*Info);
    
    return Info;
}

void UBillboardComponent::SaveComponentInfo(FActorComponentInfo& OutInfo)
{
    FBillboardComponentInfo& Info = static_cast<FBillboardComponentInfo&>(OutInfo);
    Super::SaveComponentInfo(Info);
    Info.TexturePath = Texture->path;
    Info.bOnlyForEditor = bOnlyForEditor;
}

void UBillboardComponent::LoadAndConstruct(const FActorComponentInfo& Info)
{
    Super::LoadAndConstruct(Info);
    const FBillboardComponentInfo& billboardInfo = static_cast<const FBillboardComponentInfo&>(Info);
    SetTexture(billboardInfo.TexturePath);
    bOnlyForEditor = billboardInfo.bOnlyForEditor;
}

bool UBillboardComponent::CheckPickingOnNDC(const TArray<FVector>& checkQuad, float& hitDistance)
{
    UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
    if (EditorEngine == nullptr)
    {
        return false;
    }
    
	bool result = false;
    FVector2D ClientPos = FWindowsCursor::GetClientPosition(GEngineLoop.GetDefaultWindow());
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	GEngineLoop.GraphicDevice.DeviceContext->RSGetViewports(&numViewports, &viewport);

	FVector pickPosition;
    FMatrix projectionMatrix = EditorEngine->GetLevelEditor()->GetActiveViewportClient()->GetProjectionMatrix();
	pickPosition.X = ((2.0f * ClientPos.X / viewport.Width) - 1);
	pickPosition.Y = -((2.0f * ClientPos.Y / viewport.Height) - 1);
	pickPosition.Z = 1.0f; // Near Plane

	FMatrix M = CreateBillboardMatrix();
    FMatrix V = EditorEngine->GetLevelEditor()->GetActiveViewportClient()->GetViewMatrix();;
	FMatrix P = projectionMatrix;
	FMatrix MVP = M * V * P;

	float minX = FLT_MAX;
	float maxX = FLT_MIN;
	float minY = FLT_MAX;
	float maxY = FLT_MIN;
	float avgZ = 0.0f;
	for (int i = 0; i < checkQuad.Num(); i++)
	{
		FVector4 v = FVector4(checkQuad[i].X, checkQuad[i].Y, checkQuad[i].Z, 1.0f);
		FVector4 clipPos = FMatrix::TransformVector(v, MVP);
		
		if (clipPos.W != 0)	clipPos = clipPos/clipPos.W;

		minX = FMath::Min(minX, clipPos.X);
		maxX = FMath::Max(maxX, clipPos.X);
		minY = FMath::Min(minY, clipPos.Y);
		maxY = FMath::Max(maxY, clipPos.Y);
		avgZ += clipPos.Z;
	}

	avgZ /= checkQuad.Num();

	if (pickPosition.X >= minX && pickPosition.X <= maxX &&
		pickPosition.Y >= minY && pickPosition.Y <= maxY)
	{
		float A = P.M[2][2];  // Projection Matrix의 A값 (Z 변환 계수)
		float B = P.M[3][2];  // Projection Matrix의 B값 (Z 변환 계수)

		float z_view_pick = (pickPosition.Z - B) / A; // 마우스 클릭 View 공간 Z
		float z_view_billboard = (avgZ - B) / A; // Billboard View 공간 Z

		hitDistance = 1000.0f;
		result = true;
	}

	return result;
}
