#include "DebugDepthRenderPass.h"
#include <Renderer/Renderer.h>
#include <EditorEngine.h>
#include "D3D11RHI/CBStructDefine.h"
#include "UnrealEd/EditorViewportClient.h"
#include "Renderer/VBIBTopologyMapping.h"

void FDebugDepthRenderPass::AddRenderObjectsToRenderPass(UWorld* InWorld)
{
}

void FDebugDepthRenderPass::Prepare(std::shared_ptr<FViewportClient> InViewportClient)
{
    FBaseRenderPass::Prepare(InViewportClient);

    const FRenderer& Renderer = GEngine->renderer;
    const FGraphicsDevice& Graphics = GEngine->graphicDevice;

    Graphics.DeviceContext->OMSetDepthStencilState(Renderer.GetDepthStencilState(EDepthStencilState::DepthNone), 0);

    Graphics.DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    Graphics.DeviceContext->CopyResource(Graphics.DepthCopyTexture, Graphics.DepthStencilBuffer);
    
    ID3D11SamplerState* Sampler = Renderer.GetSamplerState(ESamplerType::Point);
    Graphics.DeviceContext->PSSetSamplers(0, 1, &Sampler);
    Graphics.DeviceContext->PSSetShaderResources(0, 1, &Graphics.DepthCopySRV);

    const auto& CurRTV = Graphics.GetCurrentRenderTargetView();
    Graphics.DeviceContext->OMSetRenderTargets(1, &CurRTV, nullptr);
}

void FDebugDepthRenderPass::Execute(std::shared_ptr<FViewportClient> InViewportClient)
{
    const FGraphicsDevice& Graphics = GEngine->graphicDevice;

    UpdateCameraConstant(InViewportClient);
    UpdateScreenConstant(InViewportClient);

    Graphics.DeviceContext->Draw(4, 0);
}

void FDebugDepthRenderPass::UpdateCameraConstant(const std::shared_ptr<FViewportClient> InViewportClient)
{
    const FGraphicsDevice& Graphics = GEngine->graphicDevice;
    FRenderResourceManager* renderResourceManager = GEngine->renderer.GetResourceManager();
    std::shared_ptr<FEditorViewportClient> curEditorViewportClient = std::dynamic_pointer_cast<FEditorViewportClient>(InViewportClient);

    FCameraConstant CameraConstants;
    CameraConstants.CameraForward = FVector::ZeroVector;
    CameraConstants.CameraPos = FVector::ZeroVector;
    CameraConstants.ViewProjMatrix = FMatrix::Identity;
    CameraConstants.ProjMatrix = FMatrix::Identity;
    CameraConstants.ViewMatrix = FMatrix::Identity;
    CameraConstants.NearPlane = curEditorViewportClient->GetNearClip();
    CameraConstants.FarPlane = curEditorViewportClient->GetFarClip();

    renderResourceManager->UpdateConstantBuffer(TEXT("FCameraConstant"), &CameraConstants);
}

void FDebugDepthRenderPass::UpdateScreenConstant(const std::shared_ptr<FViewportClient> InViewportClient)
{
    const FGraphicsDevice& Graphics = GEngine->graphicDevice;
    FRenderResourceManager* renderResourceManager = GEngine->renderer.GetResourceManager();
    std::shared_ptr<FEditorViewportClient> curEditorViewportClient = std::dynamic_pointer_cast<FEditorViewportClient>(InViewportClient);

    FViewportInfo ScreenConstans;
    float Width = Graphics.screenWidth;
    float Height = Graphics.screenHeight;
    ScreenConstans.ViewportSize = { curEditorViewportClient->GetD3DViewport().Width / Width, curEditorViewportClient->GetD3DViewport().Height / Height };
    ScreenConstans.ViewportOffset = { curEditorViewportClient->GetD3DViewport().TopLeftX / Width, curEditorViewportClient->GetD3DViewport().TopLeftY / Height };

    renderResourceManager->UpdateConstantBuffer(TEXT("FViewportInfo"), &ScreenConstans);
}
