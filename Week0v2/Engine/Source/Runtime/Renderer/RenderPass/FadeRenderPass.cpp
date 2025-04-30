#include "FadeRenderPass.h"

#include "EditorEngine.h"
#include "D3D11RHI/CBStructDefine.h"
#include "D3D11RHI/GraphicDevice.h"
#include "UObject/UObjectIterator.h"

FFadeRenderPass::FFadeRenderPass(const FName& InShaderName)
    : FBaseRenderPass(InShaderName)
{

    FRenderer& Renderer = GEngine->renderer;
    FRenderResourceManager* RenderResourceManager = Renderer.GetResourceManager();
    bRender = true;
    FadeConstantBuffer = RenderResourceManager->CreateConstantBuffer(sizeof(FFadeConstants));
}

void FFadeRenderPass::AddRenderObjectsToRenderPass(UWorld* InWorld)
{
}

void FFadeRenderPass::Prepare(std::shared_ptr<FViewportClient> InViewportClient)
{
    bRender = true;
    
    if (bRender)
    {
        FBaseRenderPass::Prepare(InViewportClient);
        const FRenderer& Renderer = GEngine->renderer;

        FGraphicsDevice& Graphics = GEngine->graphicDevice;
        Graphics.SwapPingPongBuffers();

        const auto CurRTV = Graphics.GetCurrentRenderTargetView();
        Graphics.DeviceContext->OMSetRenderTargets(1, &CurRTV, nullptr);
        Graphics.DeviceContext->OMSetDepthStencilState(Renderer.GetDepthStencilState(EDepthStencilState::DepthNone), 0);

        Graphics.DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        
        ID3D11SamplerState* Sampler = Renderer.GetSamplerState(ESamplerType::Linear);
        Graphics.DeviceContext->PSSetSamplers(0, 1, &Sampler);

        const auto PreviousSRV = Graphics.GetPreviousShaderResourceView();
        Graphics.DeviceContext->PSSetShaderResources(1, 1, &PreviousSRV);
    }
}

void FFadeRenderPass::Execute(std::shared_ptr<FViewportClient> InViewportClient)
{

    if (bRender)
    {
        FGraphicsDevice& Graphics = GEngine->graphicDevice;
        Graphics.DeviceContext->Draw(6, 0);

        bRender = false;
    }
}

void FFadeRenderPass::UpdateFadeConstant()
{
}

// void FFadeRenderPass::UpdateFadeConstant()
// {
//     const FGraphicsDevice& Graphics = GEngine->graphicDevice;
//     FRenderResourceManager* renderResourceManager = GEngine->renderer.GetResourceManager();
//     std::shared_ptr<FEditorViewportClient> curEditorViewportClient = std::dynamic_pointer_cast<FEditorViewportClient>(InViewportClient);
//
//     FFogCameraConstant CameraConstants;
//     CameraConstants.InvProjMatrix = FMatrix::Inverse(curEditorViewportClient->GetProjectionMatrix());
//     CameraConstants.InvViewMatrix = FMatrix::Inverse(curEditorViewportClient->GetViewMatrix());
//     CameraConstants.CameraForward = curEditorViewportClient->ViewTransformPerspective.GetForwardVector();
//     CameraConstants.CameraPos = curEditorViewportClient->ViewTransformPerspective.GetLocation();
//     CameraConstants.NearPlane = curEditorViewportClient->GetNearClip();
//     CameraConstants.FarPlane = curEditorViewportClient->GetFarClip();
//
//     renderResourceManager->UpdateConstantBuffer(FadeConstantBuffer, &CameraConstants);
//     Graphics.DeviceContext->PSSetConstantBuffers(0, 1, &FadeConstantBuffer);
// }
