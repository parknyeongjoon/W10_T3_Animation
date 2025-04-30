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

    FadeConstantBuffer = RenderResourceManager->CreateConstantBuffer(sizeof(FFadeConstants));
}

void FFadeRenderPass::AddRenderObjectsToRenderPass(UWorld* InWorld)
{
    // for (const AActor* actor : InWorld->GetActors())
    // {
    //     for (const UActorComponent* actorComp : actor->GetComponents())
    //     {
    //         if (UStaticMeshComponent* pStaticMeshComp = Cast<UStaticMeshComponent>(actorComp))
    //         {
    //             if (!Cast<UGizmoBaseComponent>(actorComp))
    //                 StaticMesheComponents.Add(pStaticMeshComp);
    //         }
    //         
    //         if (ULightComponentBase* pGizmoComp = Cast<ULightComponentBase>(actorComp))
    //         {
    //             LightComponents.Add(pGizmoComp);
    //         }
    //     }
    // }
}

void FFadeRenderPass::PrePrepare()
{
    FGraphicsDevice& Graphics = GEngine->graphicDevice;
    FRenderer& Renderer = GEngine->renderer;
    Graphics.SwitchRTV();
    bRender = true;
    // for (const auto iter : TObjectRange<UHeightFogComponent>())
    // {
    //     if (iter->GetWorld() == GEngine->GetWorld())
    //     {
    //         FogComp = iter;
    //         Graphics.SwitchRTV();
    //         bRender = true;
    //         return;
    //     }
    // }
}

void FFadeRenderPass::Prepare(std::shared_ptr<FViewportClient> InViewportClient)
{
    if (bRender)
    {
        FBaseRenderPass::Prepare(InViewportClient);
        const FRenderer& Renderer = GEngine->renderer;
        FGraphicsDevice& Graphics = GEngine->graphicDevice;
        Graphics.ReturnRTV();

        Graphics.DeviceContext->OMSetRenderTargets(1, &Graphics.FrameBufferRTV, nullptr);
        //Graphics.DeviceContext->CopyResource(Graphics.DepthCopyTexture, Graphics.DepthStencilBuffer);
        Graphics.DeviceContext->OMSetDepthStencilState(Renderer.GetDepthStencilState(EDepthStencilState::DepthNone), 0);

        Graphics.DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


        ID3D11SamplerState* Sampler = Renderer.GetSamplerState(ESamplerType::Linear);
        Graphics.DeviceContext->PSSetSamplers(0, 1, &Sampler);
        //Graphics.DeviceContext->PSSetShaderResources(0, 1, &Graphics.DepthCopySRV);
        Graphics.DeviceContext->PSSetShaderResources(1, 1, &Graphics.SceneColorSRV);
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
