#include "DebugDepthRenderPass.h"
#include <Renderer/Renderer.h>
#include <EditorEngine.h>

#include "LaunchEngineLoop.h"
#include "Viewport.h"
#include "D3D11RHI/CBStructDefine.h"
#include "UnrealEd/EditorViewportClient.h"
#include "Renderer/VBIBTopologyMapping.h"
#include "SlateCore/Layout/SlateRect.h"

void FDebugDepthRenderPass::AddRenderObjectsToRenderPass(UWorld* World)
{
}

void FDebugDepthRenderPass::Prepare(std::shared_ptr<FViewportClient> InViewportClient)
{
    FBaseRenderPass::Prepare(InViewportClient);

    const FRenderer& Renderer = GEngineLoop.Renderer;
    const FGraphicsDevice& Graphics = GEngineLoop.GraphicDevice;

    Graphics.DeviceContext->OMSetDepthStencilState(Renderer.GetDepthStencilState(EDepthStencilState::DepthNone), 0);

    Graphics.DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    Graphics.DeviceContext->CopyResource(Graphics.GetCurrentWindowData()->DepthCopyTexture, Graphics.GetCurrentWindowData()->DepthStencilBuffer);
    
    ID3D11SamplerState* Sampler = Renderer.GetSamplerState(ESamplerType::Point);
    Graphics.DeviceContext->PSSetSamplers(0, 1, &Sampler);
    Graphics.DeviceContext->PSSetShaderResources(0, 1, &Graphics.GetCurrentWindowData()->DepthCopySRV);

    const auto& CurRTV = Graphics.GetCurrentRenderTargetView();
    Graphics.DeviceContext->OMSetRenderTargets(1, &CurRTV, nullptr);
}

void FDebugDepthRenderPass::Execute(const std::shared_ptr<FViewportClient> InViewportClient)
{
    const FGraphicsDevice& Graphics = GEngineLoop.GraphicDevice;

    UpdateCameraConstant(InViewportClient);
    UpdateScreenConstant(InViewportClient);

    Graphics.DeviceContext->Draw(4, 0);
}

void FDebugDepthRenderPass::UpdateCameraConstant(const std::shared_ptr<FViewportClient> InViewportClient)
{
    FRenderResourceManager* renderResourceManager = GEngineLoop.Renderer.GetResourceManager();
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
