#include "FogRenderPass.h"
#include <Define.h>
#include <EditorEngine.h>
#include <UObject/UObjectIterator.h>
#include <D3D11RHI/CBStructDefine.h>

#include "LaunchEngineLoop.h"
#include "Viewport.h"
#include "Components/PrimitiveComponents/HeightFogComponent.h"
#include "Renderer/Renderer.h"
#include "SlateCore/Layout/SlateRect.h"
#include "UnrealEd/EditorViewportClient.h"

extern UEngine* GEngine;

FFogRenderPass::FFogRenderPass(const FName& InShaderName)
    :FBaseRenderPass(InShaderName)
{
    if (FogCameraConstantBuffer)
        return;
    FGraphicsDevice& Graphics = GEngineLoop.GraphicDevice;
    D3D11_BUFFER_DESC cbDesc = {};
    cbDesc.ByteWidth = sizeof(FFogCameraConstant);
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbDesc.MiscFlags = 0;
    cbDesc.StructureByteStride = 0;

    HRESULT hr = Graphics.Device->CreateBuffer(&cbDesc, nullptr, &FogCameraConstantBuffer);
    if (FAILED(hr))
    {
        // 에러 처리
    }
}

void FFogRenderPass::AddRenderObjectsToRenderPass(UWorld* World)
{
    bRender = false;
    FGraphicsDevice& Graphics = GEngineLoop.GraphicDevice;
    for (const auto iter : TObjectRange<UHeightFogComponent>())
    {
        if (iter->GetWorld() == World)
        {
            FogComp = iter;
            Graphics.SwapPingPongBuffers();
            bRender = true;
            return;
        }
    }
}

void FFogRenderPass::ClearRenderObjects()
{
    FBaseRenderPass::ClearRenderObjects();

    FogComp = nullptr;
    bRender = false;
}

void FFogRenderPass::Prepare(std::shared_ptr<FViewportClient> InViewportClient)
{    
    if (bRender)
    {
        FBaseRenderPass::Prepare(InViewportClient);
        const FRenderer& Renderer = GEngineLoop.Renderer;
        FGraphicsDevice& Graphics = GEngineLoop.GraphicDevice;

        const auto CurRTV = Graphics.GetCurrentRenderTargetView();
        Graphics.DeviceContext->OMSetRenderTargets(1, &CurRTV, nullptr);
        Graphics.DeviceContext->CopyResource(Graphics.DepthCopyTexture, Graphics.DepthStencilBuffer);
        Graphics.DeviceContext->OMSetDepthStencilState(Renderer.GetDepthStencilState(EDepthStencilState::DepthNone), 0);

        Graphics.DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


        ID3D11SamplerState* Sampler = Renderer.GetSamplerState(ESamplerType::Linear);
        Graphics.DeviceContext->PSSetSamplers(0, 1, &Sampler);

        const auto PreviousSRV = Graphics.GetPreviousShaderResourceView();
        Graphics.DeviceContext->PSSetShaderResources(0, 1, &Graphics.DepthCopySRV);
        Graphics.DeviceContext->PSSetShaderResources(1, 1, &PreviousSRV);
    }
}

void FFogRenderPass::Execute(std::shared_ptr<FViewportClient> InViewportClient)
{
    if (bRender)
    {
        FGraphicsDevice& Graphics = GEngineLoop.GraphicDevice;

        UpdateCameraConstant(InViewportClient);
        UpdateScreenConstant(InViewportClient);
        UpdateFogConstant(InViewportClient);

        Graphics.DeviceContext->Draw(6, 0);

        bRender = false;
    }
}

void FFogRenderPass::UpdateCameraConstant(const std::shared_ptr<FViewportClient> InViewportClient) const
{
    const FGraphicsDevice& Graphics = GEngineLoop.GraphicDevice;
    FRenderResourceManager* renderResourceManager = GEngineLoop.Renderer.GetResourceManager();
    std::shared_ptr<FEditorViewportClient> curEditorViewportClient = std::dynamic_pointer_cast<FEditorViewportClient>(InViewportClient);

    FFogCameraConstant CameraConstants;
    CameraConstants.InvProjMatrix = FMatrix::Inverse(curEditorViewportClient->GetProjectionMatrix());
    CameraConstants.InvViewMatrix = FMatrix::Inverse(curEditorViewportClient->GetViewMatrix());
    CameraConstants.CameraForward = curEditorViewportClient->ViewTransformPerspective.GetForwardVector();
    CameraConstants.CameraPos = curEditorViewportClient->ViewTransformPerspective.GetLocation();
    CameraConstants.NearPlane = curEditorViewportClient->GetNearClip();
    CameraConstants.FarPlane = curEditorViewportClient->GetFarClip();

    renderResourceManager->UpdateConstantBuffer(FogCameraConstantBuffer, &CameraConstants);

    Graphics.DeviceContext->PSSetConstantBuffers(0, 1, &FogCameraConstantBuffer);
}

void FFogRenderPass::UpdateScreenConstant(std::shared_ptr<FViewportClient> InViewportClient)
{
    const FGraphicsDevice& Graphics = GEngineLoop.GraphicDevice;
    FRenderResourceManager* renderResourceManager = GEngineLoop.Renderer.GetResourceManager();
    std::shared_ptr<FEditorViewportClient> curEditorViewportClient = std::dynamic_pointer_cast<FEditorViewportClient>(InViewportClient);

    FViewportInfo ScreenConstants;
    float Width = Graphics.GetCurrentWindowData()->ScreenHeight;
    float Height = Graphics.GetCurrentWindowData()->ScreenHeight;
    ScreenConstants.ViewportSize = FVector2D { curEditorViewportClient->GetViewport()->GetFSlateRect().Width / Width, curEditorViewportClient->GetViewport()->GetFSlateRect().Height / Height };
    ScreenConstants.ViewportOffset = FVector2D { curEditorViewportClient->GetViewport()->GetFSlateRect().LeftTopX / Width, curEditorViewportClient->GetViewport()->GetFSlateRect().LeftTopY / Height };

    renderResourceManager->UpdateConstantBuffer(TEXT("FViewportInfo"), &ScreenConstants);
}

void FFogRenderPass::UpdateFogConstant(const std::shared_ptr<FViewportClient> InViewportClient) const
{
    const FGraphicsDevice& Graphics = GEngineLoop.GraphicDevice;
    FRenderResourceManager* renderResourceManager = GEngineLoop.Renderer.GetResourceManager();

    FFogParams FogParams;
    FogParams.FogColor = FogComp->GetFogColor();
    FogParams.FogDensity = FogComp->GetFogDensity();
    FogParams.FogStart = FogComp->GetFogStart();
    FogParams.FogEnd = FogComp->GetFogEnd();
    FogParams.FogZPosition = FogComp->GetFogZPosition();
    FogParams.FogBaseHeight = FogComp->GetFogBaseHeight();
    FogParams.HeightFallOff = FogComp->GetHeightFallOff();
    FogParams.bIsHeightFog = FogComp->IsHeightFog();
    FogParams.FogMaxOpacity = FogComp->GetMaxOpacity();
    FogParams.LightShaftDensity = FogComp->GetLightShaftDensity();

    renderResourceManager->UpdateConstantBuffer(TEXT("FFogParams"), &FogParams);
}
