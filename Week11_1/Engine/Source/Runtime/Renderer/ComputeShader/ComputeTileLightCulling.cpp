#include "ComputeTileLightCulling.h"

#include "LaunchEngineLoop.h"
#include "Viewport.h"
#include "Components/LightComponents/DirectionalLightComponent.h"
#include "Components/LightComponents/PointLightComponent.h"
#include "D3D11RHI/CBStructDefine.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Renderer/Renderer.h"
#include "SlateCore/Layout/SlateRect.h"
#include "UnrealEd/EditorViewportClient.h"
#include "UObject/UObjectIterator.h"

FComputeTileLightCulling::FComputeTileLightCulling(const FName& InShaderName)
{
    FRenderResourceManager* renderResourceManager = GEngineLoop.Renderer.GetResourceManager();
    FGraphicsDevice& Graphics = GEngineLoop.GraphicDevice;
    
    ID3D11Buffer* SB = nullptr;
    ID3D11ShaderResourceView* SBSRV = nullptr;
    ID3D11UnorderedAccessView* SBUAV = nullptr;

    ID3D11UnorderedAccessView* TileCullingUAV = renderResourceManager->GetStructuredBufferUAV("TileLightCulling");
    if (TileCullingUAV == nullptr)
    {
        SB = renderResourceManager->CreateUAVStructuredBuffer<UINT>(1);
        SBSRV = renderResourceManager->CreateBufferSRV(SB, 1);
        SBUAV = renderResourceManager->CreateBufferUAV(SB, 1);  

        renderResourceManager->AddOrSetUAVStructuredBuffer(TEXT("TileLightCulling"), SB);
        renderResourceManager->AddOrSetSRVStructuredBufferSRV(TEXT("TileLightCulling"), SBSRV);
        renderResourceManager->AddOrSetUAVStructuredBufferUAV(TEXT("TileLightCulling"), SBUAV);
    }
}

void FComputeTileLightCulling::AddRenderObjectsToRenderPass(UWorld* InWorld)
{
    LightComponents.Empty();
    
    if (InWorld->WorldType == EWorldType::Editor || InWorld->WorldType == EWorldType::EditorPreview)
    {
        for (const auto iter : TObjectRange<USceneComponent>())
        {
            if (iter->GetWorld() != InWorld)
            {
                continue;
            }
            
            if (ULightComponentBase* pGizmoComp = Cast<ULightComponentBase>(iter))
            {
                LightComponents.Add(pGizmoComp);
            }
        }
    }
}

void FComputeTileLightCulling::OnResize(int screenWidth, int screenHeight)
{
}

void FComputeTileLightCulling::Dispatch(const std::shared_ptr<FViewportClient> InViewportClient)
{
    //Dispatch
    FRenderResourceManager* renderResourceManager = GEngineLoop.Renderer.GetResourceManager();
    FGraphicsDevice& Graphics = GEngineLoop.GraphicDevice;

    ID3D11UnorderedAccessView* TileCullingUAV = renderResourceManager->GetStructuredBufferUAV("TileLightCulling");
        
    FEditorViewportClient* ViewPort = dynamic_cast<FEditorViewportClient*>(InViewportClient.get());
    
    int screenWidth = ViewPort->GetViewport()->GetFSlateRect().Width;  // 화면 가로 픽셀 수
    int screenHeight = ViewPort->GetViewport()->GetFSlateRect().Height;  // 화면 세로 픽셀 수
    
    // 타일 크기 (예: 16x16 픽셀)
    const int TILE_SIZE_X = 16;
    const int TILE_SIZE_Y = 16;

    // 타일 개수 계산
    int numTilesX = (screenWidth + TILE_SIZE_X - 1) / TILE_SIZE_X; // 1024/16=64
    int numTilesY = (screenHeight + TILE_SIZE_Y - 1) / TILE_SIZE_Y; // 768/16=48
    
    if (PreviousTileCount.X != numTilesX || PreviousTileCount.Y != numTilesY)
    {
        ID3D11Buffer* SB = nullptr;
        ID3D11ShaderResourceView* SBSRV = nullptr;
        ID3D11UnorderedAccessView* SBUAV = nullptr;

        PreviousTileCount.X = numTilesX;
        PreviousTileCount.Y = numTilesY;
        
        int MaxPointLightCount = 16;
        int UAVElementCount = MaxPointLightCount * numTilesX * numTilesY;
        
        SB = renderResourceManager->CreateUAVStructuredBuffer<UINT>(UAVElementCount);
        SBSRV = renderResourceManager->CreateBufferSRV(SB, UAVElementCount);
        SBUAV = renderResourceManager->CreateBufferUAV(SB, UAVElementCount);  

        renderResourceManager->AddOrSetSRVStructuredBufferSRV(TEXT("TileLightCulling"), SBSRV);
        renderResourceManager->AddOrSetUAVStructuredBufferUAV(TEXT("TileLightCulling"), SBUAV);
        renderResourceManager->AddOrSetUAVStructuredBuffer(TEXT("TileLightCulling"), SB);

        TileCullingUAV = SBUAV;
    }

    Graphics.DeviceContext->CSSetShader(renderResourceManager->GetComputeShader("TileLightCulling"), nullptr, 0);
    Graphics.DeviceContext->CSSetUnorderedAccessViews(0, 1, &TileCullingUAV, nullptr);

    UpdateLightConstants();

    UpdateComputeConstants(InViewportClient, numTilesX, numTilesY);

    // 전체화면을 타일크기로 나눈 타일 단위로 각각의 워크그룹이 처리한다. 즉 16x16픽셀의 한 타일을 쓰레드로 관리
    Graphics.DeviceContext->Dispatch(numTilesX, numTilesY, 1);
    
    //해제
    ID3D11UnorderedAccessView* nullUAV[1] = { nullptr };
    Graphics.DeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
}

void FComputeTileLightCulling::UpdateLightConstants()
{
    FRenderResourceManager* renderResourceManager = GEngineLoop.Renderer.GetResourceManager();
    FGraphicsDevice& Graphics = GEngineLoop.GraphicDevice;
    
    FLightingConstants LightConstant;
    uint32 PointLightCount = 0;

    for (ULightComponentBase* Comp : LightComponents)
    {
        if (UPointLightComponent* PointLightComp = dynamic_cast<UPointLightComponent*>(Comp))
        {
            LightConstant.PointLights[PointLightCount].Color = PointLightComp->GetLightColor();
            LightConstant.PointLights[PointLightCount].Intensity = PointLightComp->GetIntensity();
            LightConstant.PointLights[PointLightCount].Position = PointLightComp->GetWorldLocation();
            LightConstant.PointLights[PointLightCount].Radius = PointLightComp->GetRadius();
            LightConstant.PointLights[PointLightCount].AttenuationFalloff = PointLightComp->GetAttenuationFalloff();
            PointLightCount++;
            continue;
        }

        if (UDirectionalLightComponent* DirectionalLightComp = dynamic_cast<UDirectionalLightComponent*>(Comp))
        {
            LightConstant.DirLight.Color = DirectionalLightComp->GetLightColor();
            LightConstant.DirLight.Intensity = DirectionalLightComp->GetIntensity();
            LightConstant.DirLight.Direction = DirectionalLightComp->GetWorldForwardVector();
        }
    }

    LightConstant.NumPointLights = PointLightCount;

    ID3D11Buffer* LightConstantBuffer = renderResourceManager->GetConstantBuffer(TEXT("FLightingConstants"));
    
    Graphics.DeviceContext->CSSetConstantBuffers(1, 1, &LightConstantBuffer);
    renderResourceManager->UpdateConstantBuffer(LightConstantBuffer, &LightConstant);
}

void FComputeTileLightCulling::UpdateComputeConstants(const std::shared_ptr<FViewportClient> InViewportClient, int NumTileX, int NumTileY)
{
    FRenderResourceManager* renderResourceManager = GEngineLoop.Renderer.GetResourceManager();
    FGraphicsDevice& Graphics = GEngineLoop.GraphicDevice;

    FEditorViewportClient* EditorViewPortClient = dynamic_cast<FEditorViewportClient*>(InViewportClient.get());
    
    FMatrix InvView = FMatrix::Identity;
    FMatrix InvProj = FMatrix::Identity;
    std::shared_ptr<FEditorViewportClient> curEditorViewportClient = std::dynamic_pointer_cast<FEditorViewportClient>(InViewportClient);
    if (curEditorViewportClient != nullptr)
    {
        InvView = FMatrix::Inverse(curEditorViewportClient->GetViewMatrix());
        InvProj = FMatrix::Inverse(curEditorViewportClient->GetProjectionMatrix());
    }
    
    FComputeConstants ComputeConstants;
    
    ComputeConstants.screenHeight = EditorViewPortClient->GetViewport()->GetFSlateRect().Height;
    ComputeConstants.screenWidth = EditorViewPortClient->GetViewport()->GetFSlateRect().Width;
    ComputeConstants.InverseProj = InvProj;
    ComputeConstants.InverseView = InvView;
    ComputeConstants.tileCountX = NumTileX;
    ComputeConstants.tileCountY = NumTileY;

    ID3D11Buffer* ComputeConstantBuffer = renderResourceManager->GetConstantBuffer(TEXT("FComputeConstants"));
    
    Graphics.DeviceContext->CSSetConstantBuffers(0, 1, &ComputeConstantBuffer);
    renderResourceManager->UpdateConstantBuffer(ComputeConstantBuffer, &ComputeConstants);
}