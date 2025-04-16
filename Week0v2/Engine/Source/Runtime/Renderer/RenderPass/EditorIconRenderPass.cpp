#include "EditorIconRenderPass.h"

#include "EditorEngine.h"
#include "Components/LightComponent.h"
#include "Components/UBillboardComponent.h"
#include "D3D11RHI/CBStructDefine.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Renderer/Renderer.h"
#include "UnrealEd/EditorViewportClient.h"

FEditorIconRenderPass::~FEditorIconRenderPass()
{
    BillboardComponents.Empty();
}

void FEditorIconRenderPass::AddRenderObjectsToRenderPass(UWorld* InLevel)
{
    if (InLevel->WorldType != EWorldType::Editor)
    {
        return;
    }

    for (const auto actor : InLevel->GetActors())
    {
        for (const auto comp : actor->GetComponents())
        {
            if (UBillboardComponent* billboardComp = Cast<UBillboardComponent>(comp))
            {
                BillboardComponents.Add(billboardComp);
            }
        }
    }
}

void FEditorIconRenderPass::Prepare(const std::shared_ptr<FViewportClient> InViewportClient)
{
    FBaseRenderPass::Prepare(InViewportClient);
    const FRenderer& Renderer = GEngine->renderer;
    const FGraphicsDevice& Graphics = GEngine->graphicDevice;
    
    Graphics.DeviceContext->OMSetDepthStencilState(Renderer.GetDepthStencilState(EDepthStencilState::DepthNone), 0);
    Graphics.DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 정정 연결 방식 설정
    Graphics.DeviceContext->RSSetState(Renderer.GetRasterizerState(ERasterizerState::SolidBack));

    // RTVs 배열의 유효성을 확인합니다.
    if (Graphics.RTVs[0] != nullptr)
    {
        Graphics.DeviceContext->OMSetRenderTargets(1, &Graphics.RTVs[0], Graphics.DepthStencilView); // 렌더 타겟 설정
    }
    else
    {
        // RTVs[0]이 nullptr인 경우에 대한 처리
        // 예를 들어, 기본 렌더 타겟을 설정하거나 오류를 기록할 수 있습니다.
        // Graphics.DeviceContext->OMSetRenderTargets(1, &Graphics.FrameBufferRTV, Graphics.DepthStencilView);
    }

    Graphics.DeviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff); // 블렌딩 상태 설정, 기본 블렌딩 상태임
    
    ID3D11SamplerState* linearSampler = Renderer.GetSamplerState(ESamplerType::Linear);
    Graphics.DeviceContext->PSSetSamplers(static_cast<uint32>(ESamplerType::Linear), 1, &linearSampler);
}

void FEditorIconRenderPass::Execute(const std::shared_ptr<FViewportClient> InViewportClient)
{
    FMatrix View = FMatrix::Identity;
    FMatrix Proj = FMatrix::Identity;

    FGraphicsDevice& Graphics = GEngine->graphicDevice;
    FRenderer& Renderer = GEngine->renderer;
    FRenderResourceManager* renderResourceManager = GEngine->renderer.GetResourceManager();

    std::shared_ptr<FEditorViewportClient> curEditorViewportClient = std::dynamic_pointer_cast<FEditorViewportClient>(InViewportClient);
    if (curEditorViewportClient != nullptr)
    {
        View = curEditorViewportClient->GetViewMatrix();
        Proj = curEditorViewportClient->GetProjectionMatrix();
    }
    
    FSceneConstant SceneConstants;
    SceneConstants.ViewMatrix = View;
    SceneConstants.ProjMatrix = Proj;
    SceneConstants.CameraPos = curEditorViewportClient->ViewTransformPerspective.GetLocation();
    SceneConstants.CameraLookAt = curEditorViewportClient->ViewTransformPerspective.GetLookAt();

    renderResourceManager->UpdateConstantBuffer(TEXT("FSceneConstant"), &SceneConstants);
    
    for (const UBillboardComponent* item : BillboardComponents)
    {
        FDebugIconConstant DebugConstant;
        DebugConstant.IconPosition = item->GetComponentLocation();
        DebugConstant.IconScale = 1;

        renderResourceManager->UpdateConstantBuffer(TEXT("FDebugIconConstant"), &DebugConstant);

        Graphics.DeviceContext->PSSetShaderResources(0, 1, &item->Texture->TextureSRV);
        
        Graphics.DeviceContext->Draw(6, 0); // 내부에서 버텍스 사용중
    }
}

void FEditorIconRenderPass::ClearRenderObjects()
{
    BillboardComponents.Empty();
}
