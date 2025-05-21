#include "BlurRenderPass.h"

#include "FadeRenderPass.h"

#include "EditorEngine.h"
#include "LaunchEngineLoop.h"
#include "Viewport.h"
#include "D3D11RHI/CBStructDefine.h"
#include "D3D11RHI/GraphicDevice.h"
#include "LevelEditor/SLevelEditor.h"
#include "Renderer/Renderer.h"
#include "SlateCore/Layout/SlateRect.h"
#include "UnrealEd/EditorViewportClient.h"

class FRenderResourceManager;

FBlurRenderPass::FBlurRenderPass(const FName& InShaderName)
    : FBaseRenderPass(InShaderName)
{
    FRenderer& Renderer = GEngineLoop.Renderer;
    FRenderResourceManager* RenderResourceManager = Renderer.GetResourceManager();
    bRender = true;
    BlurConstantBuffer = RenderResourceManager->CreateConstantBuffer(sizeof(FFadeConstants));
}

void FBlurRenderPass::AddRenderObjectsToRenderPass(UWorld* World)
{
}

void FBlurRenderPass::Prepare(std::shared_ptr<FViewportClient> InViewportClient)
{
    bRender = true;
    if (bRender)
    {
        FBaseRenderPass::Prepare(InViewportClient);
        const FRenderer& Renderer = GEngineLoop.Renderer;
        FGraphicsDevice& Graphics = GEngineLoop.GraphicDevice;
        Graphics.SwapPingPongBuffers();

        const auto CurRTV = Graphics.GetCurrentRenderTargetView();
        Graphics.DeviceContext->OMSetRenderTargets(1, &CurRTV, nullptr);
        Graphics.DeviceContext->OMSetDepthStencilState(Renderer.GetDepthStencilState(EDepthStencilState::DepthNone), 0);

        Graphics.DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        
        ID3D11SamplerState* Sampler = Renderer.GetSamplerState(ESamplerType::Point);
        Graphics.DeviceContext->PSSetSamplers(0, 1, &Sampler);

        const auto PreviousSRV = Graphics.GetPreviousShaderResourceView();
        Graphics.DeviceContext->PSSetShaderResources(0, 1, &PreviousSRV);
    }
}

void FBlurRenderPass::Execute(const std::shared_ptr<FViewportClient> InViewportClient)
{
    FGraphicsDevice& Graphics = GEngineLoop.GraphicDevice;

    if (bRender)
    {
        auto viewPort = std::dynamic_pointer_cast<FEditorViewportClient>(InViewportClient);
        if (UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine))
        {
            UpdateBlurConstant(EditorEngine->testBlurStrength,1 / viewPort->GetViewport()->GetFSlateRect().Width, 1 / viewPort->GetViewport()->GetFSlateRect().Height);
        }
        Graphics.DeviceContext->Draw(6, 0);

        bRender = false;
    }
}

void FBlurRenderPass::UpdateBlurConstant(float BlurStrength, float TexelX, float TexelY) const
{
    const FGraphicsDevice& Graphics = GEngineLoop.GraphicDevice;
    FRenderResourceManager* renderResourceManager = GEngineLoop.Renderer.GetResourceManager();

    FBlurConstants BlurConstants;
    BlurConstants.BlurStrength = BlurStrength;
    BlurConstants.TexelSizeX = TexelX;
    BlurConstants.TexelSizeY = TexelY;
    
    renderResourceManager->UpdateConstantBuffer(BlurConstantBuffer, &BlurConstants);
    Graphics.DeviceContext->PSSetConstantBuffers(0, 1, &BlurConstantBuffer);
}
