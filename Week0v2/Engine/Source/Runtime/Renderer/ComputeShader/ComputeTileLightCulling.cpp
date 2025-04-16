#include "ComputeTileLightCulling.h"

#include "EditorEngine.h"
#include "UnrealClient.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/LightComponent.h"
#include "Components/PointLightComponent.h"
#include "D3D11RHI/CBStructDefine.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Renderer/Renderer.h"
#include "Renderer/VBIBTopologyMapping.h"
#include "UnrealEd/EditorViewportClient.h"
#include "UObject/UObjectIterator.h"

extern UEditorEngine* GEngine;

void FComputeTileLightCulling::AddRenderObjectsToRenderPass(UWorld* InWorld)
{
    LightComponents.Empty();
    
    if (InWorld->WorldType == EWorldType::Editor)
    {
        for (const auto iter : TObjectRange<USceneComponent>())
        {
            if (ULightComponentBase* pGizmoComp = Cast<ULightComponentBase>(iter))
            {
                LightComponents.Add(pGizmoComp);
            }
        }
    }
}

void FComputeTileLightCulling::Dispatch(const std::shared_ptr<FViewportClient> InViewportClient)
{
    //Dispatch
    FRenderResourceManager* renderResourceManager = GEngine->renderer.GetResourceManager();
    FGraphicsDevice& Graphics = GEngine->graphicDevice;

    ID3D11UnorderedAccessView* TileCullingUAV = renderResourceManager->GetStructuredBufferUAV("TileLightCulling");
    
    if (TileCullingUAV == nullptr)
    {
        ID3D11Buffer* SB = nullptr;
        ID3D11ShaderResourceView* SBSRV = nullptr;
        ID3D11UnorderedAccessView* SBUAV = nullptr;

        int MaxPointLightCount = 16;
        int UAVElementCount = MaxPointLightCount * XTileCount * YTileCount;
        
        SB = renderResourceManager->CreateUAVStructuredBuffer<UINT>(UAVElementCount);
        SBSRV = renderResourceManager->CreateBufferSRV(SB, UAVElementCount);
        SBUAV = renderResourceManager->CreateBufferUAV(SB, UAVElementCount);

        renderResourceManager->AddOrSetSRVStructuredBuffer(TEXT("TileLightCulling"), SB);
        renderResourceManager->AddOrSetSRVStructuredBufferSRV(TEXT("TileLightCulling"), SBSRV);
        renderResourceManager->AddOrSetUAVStructuredBuffer(TEXT("TileLightCulling"), SB);
        renderResourceManager->AddOrSetUAVStructuredBufferUAV(TEXT("TileLightCulling"), SBUAV);

        TileCullingUAV = SBUAV;
    }

    Graphics.DeviceContext->CSSetShader(renderResourceManager->GetComputeShader("TileLightCulling"), nullptr, 0);
    Graphics.DeviceContext->CSSetUnorderedAccessViews(0, 1, &TileCullingUAV, nullptr);

    UpdateLightConstants();

    UpdateComputeConstants(InViewportClient);

    //그룹 나누는 작업
    Graphics.DeviceContext->Dispatch(XTileCount, YTileCount, 1);
    
    //해제
    ID3D11UnorderedAccessView* nullUAV[1] = { nullptr };
    Graphics.DeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
}

void FComputeTileLightCulling::UpdateLightConstants()
{
    FRenderResourceManager* renderResourceManager = GEngine->renderer.GetResourceManager();
    FGraphicsDevice& Graphics = GEngine->graphicDevice;
    
    FLightingConstants LightConstant;
    uint32 DirectionalLightCount = 0;
    uint32 PointLightCount = 0;

    for (ULightComponentBase* Comp : LightComponents)
    {
        UPointLightComponent* PointLightComp = dynamic_cast<UPointLightComponent*>(Comp);

        if (PointLightComp)
        {
            LightConstant.PointLights[PointLightCount].Color = PointLightComp->GetColor();
            LightConstant.PointLights[PointLightCount].Intensity = PointLightComp->GetIntensity();
            LightConstant.PointLights[PointLightCount].Position = PointLightComp->GetWorldLocation();
            LightConstant.PointLights[PointLightCount].Radius = PointLightComp->GetRadius();
            LightConstant.PointLights[PointLightCount].AttenuationFalloff = PointLightComp->GetAttenuationFalloff();
            PointLightCount++;
            continue;
        }

        UDirectionalLightComponent* DirectionalLightComp = dynamic_cast<UDirectionalLightComponent*>(Comp);
        if (DirectionalLightComp)
        {
            LightConstant.DirLights[DirectionalLightCount].Color = DirectionalLightComp->GetColor();
            LightConstant.DirLights[DirectionalLightCount].Intensity = DirectionalLightComp->GetIntensity();
            LightConstant.DirLights[DirectionalLightCount].Direction = DirectionalLightComp->GetForwardVector();
            DirectionalLightCount++;
            continue;
        }
    }

    LightConstant.NumPointLights = PointLightCount;
    LightConstant.NumDirectionalLights = DirectionalLightCount;

    ID3D11Buffer* LightConstantBuffer = renderResourceManager->GetConstantBuffer(TEXT("FLightingConstants"));
    
    Graphics.DeviceContext->CSSetConstantBuffers(1, 1, &LightConstantBuffer);
    renderResourceManager->UpdateConstantBuffer(LightConstantBuffer, &LightConstant);
}

void FComputeTileLightCulling::UpdateComputeConstants(const std::shared_ptr<FViewportClient> InViewportClient)
{
    FRenderResourceManager* renderResourceManager = GEngine->renderer.GetResourceManager();
    FGraphicsDevice& Graphics = GEngine->graphicDevice;

    FEditorViewportClient* ViewPort = dynamic_cast<FEditorViewportClient*>(InViewportClient.get());
    
    FMatrix InvView = FMatrix::Identity;
    FMatrix InvProj = FMatrix::Identity;
    std::shared_ptr<FEditorViewportClient> curEditorViewportClient = std::dynamic_pointer_cast<FEditorViewportClient>(InViewportClient);
    if (curEditorViewportClient != nullptr)
    {
        InvView = FMatrix::Inverse(curEditorViewportClient->GetViewMatrix());
        InvProj = FMatrix::Inverse(curEditorViewportClient->GetProjectionMatrix());
    }
    
    FComputeConstants ComputeConstants;
    
    ComputeConstants.screenHeight = ViewPort->GetViewport()->GetScreenRect().Height;
    ComputeConstants.screenWidth = ViewPort->GetViewport()->GetScreenRect().Width;
    ComputeConstants.InverseProj = InvProj;
    ComputeConstants.InverseView = InvView;
    ComputeConstants.tileCountX = XTileCount;
    ComputeConstants.tileCountY = YTileCount;

    ID3D11Buffer* ComputeConstantBuffer = renderResourceManager->GetConstantBuffer(TEXT("FComputeConstants"));
    
    Graphics.DeviceContext->CSSetConstantBuffers(0, 1, &ComputeConstantBuffer);
    renderResourceManager->UpdateConstantBuffer(ComputeConstantBuffer, &ComputeConstants);
}