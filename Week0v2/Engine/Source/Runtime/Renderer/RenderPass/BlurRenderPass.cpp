#include "BlurRenderPass.h"

#include "FadeRenderPass.h"

#include "EditorEngine.h"
#include "D3D11RHI/CBStructDefine.h"
#include "D3D11RHI/GraphicDevice.h"
#include "UObject/UObjectIterator.h"

FBlurRenderPass::FBlurRenderPass(const FName& InShaderName)
    : FBaseRenderPass(InShaderName)
{

    FRenderer& Renderer = GEngine->renderer;
    FRenderResourceManager* RenderResourceManager = Renderer.GetResourceManager();
    bRender = true;
    BlurConstantBuffer = RenderResourceManager->CreateConstantBuffer(sizeof(FFadeConstants));
}

void FBlurRenderPass::AddRenderObjectsToRenderPass(UWorld* InWorld)
{
}

void FBlurRenderPass::Prepare(std::shared_ptr<FViewportClient> InViewportClient)
{
    bRender = true; // TODO: 필요한가?
    if (bRender)
    {
        FBaseRenderPass::Prepare(InViewportClient);
        const FRenderer& Renderer = GEngine->renderer;
        FGraphicsDevice& Graphics = GEngine->graphicDevice;

        const auto CurRTV = Graphics.GetCurrentRenderTargetView();
        Graphics.DeviceContext->OMSetRenderTargets(1, &CurRTV, nullptr);
        Graphics.DeviceContext->OMSetDepthStencilState(Renderer.GetDepthStencilState(EDepthStencilState::DepthNone), 0);

        Graphics.DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


        ID3D11SamplerState* Sampler = Renderer.GetSamplerState(ESamplerType::Point);
        Graphics.DeviceContext->PSSetSamplers(0, 1, &Sampler);

        const auto PreviousSRV = Graphics.GetPreviousShaderResourceView();
        Graphics.DeviceContext->PSSetShaderResources(1, 1, &PreviousSRV);
    }
}

void FBlurRenderPass::Execute(std::shared_ptr<FViewportClient> InViewportClient)
{
    FGraphicsDevice& Graphics = GEngine->graphicDevice;

    if (bRender)
    {
        Graphics.DeviceContext->Draw(6, 0);

        bRender = false;
    }

    Graphics.SwapPingPongBuffers();
}

void FBlurRenderPass::UpdateBlurConstant()
{
}
