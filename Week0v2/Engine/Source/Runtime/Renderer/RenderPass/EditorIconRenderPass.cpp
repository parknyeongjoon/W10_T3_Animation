#include "EditorIconRenderPass.h"

#include "EditorEngine.h"
#include "LaunchEngineLoop.h"
#include "Components/PrimitiveComponents/UBillboardComponent.h"
#include "D3D11RHI/CBStructDefine.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Renderer/Renderer.h"
#include "UnrealEd/EditorViewportClient.h"
#include "UObject/UObjectIterator.h"

extern UEngine* GEngine;

FEditorIconRenderPass::~FEditorIconRenderPass()
{
    BillboardComponents.Empty();
}

void FEditorIconRenderPass::AddRenderObjectsToRenderPass()
{
    for (UBillboardComponent* BillboardComponent : TObjectRange<UBillboardComponent>())
    {
        if ((BillboardComponent->GetWorld()->WorldType != EWorldType::Editor && BillboardComponent->bOnlyForEditor == true) || BillboardComponent->GetWorld() != GEngine->GetWorld())
        {
            continue;
        }
        
        BillboardComponents.Add(BillboardComponent);
    }
}

void FEditorIconRenderPass::Prepare(FRenderer* Renderer, const std::shared_ptr<FViewportClient> InViewportClient, const FString& InShaderName)
{
    FBaseRenderPass::Prepare(Renderer, InViewportClient, InShaderName);
    const FGraphicsDevice& Graphics = GEngineLoop.GraphicDevice;
    
    Graphics.DeviceContext->OMSetDepthStencilState(Renderer->GetDepthStencilState(EDepthStencilState::DepthNone), 0);
    Graphics.DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 정정 연결 방식 설정
    Graphics.DeviceContext->RSSetState(Renderer->GetRasterizerState(ERasterizerState::SolidBack));

    // RTVs 배열의 유효성을 확인합니다.
    const auto CurRTV = Graphics.GetCurrentRenderTargetView();
    if (CurRTV != nullptr)
    {
        Graphics.DeviceContext->OMSetRenderTargets(1, &CurRTV, Graphics.DepthStencilView); // 렌더 타겟 설정
    }
    else
    {
        // RTVs[0]이 nullptr인 경우에 대한 처리
        // 예를 들어, 기본 렌더 타겟을 설정하거나 오류를 기록할 수 있습니다.
        // Graphics.DeviceContext->OMSetRenderTargets(1, &Graphics.FrameBufferRTV, Graphics.DepthStencilView);
    }

    Graphics.DeviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff); // 블렌딩 상태 설정, 기본 블렌딩 상태임
    
    ID3D11SamplerState* linearSampler = Renderer->GetSamplerState(ESamplerType::Linear);
    Graphics.DeviceContext->PSSetSamplers(static_cast<uint32>(ESamplerType::Linear), 1, &linearSampler);
}

void FEditorIconRenderPass::Execute(const std::shared_ptr<FViewportClient> InViewportClient)
{
    FMatrix View = FMatrix::Identity;
    FMatrix Proj = FMatrix::Identity;

    FGraphicsDevice& Graphics = GEngineLoop.GraphicDevice;
    FRenderResourceManager* renderResourceManager = GEngineLoop.Renderer.GetResourceManager();

    std::shared_ptr<FEditorViewportClient> curEditorViewportClient = std::dynamic_pointer_cast<FEditorViewportClient>(InViewportClient);
    if (curEditorViewportClient != nullptr)
    {
        View = curEditorViewportClient->GetViewMatrix();
        Proj = curEditorViewportClient->GetProjectionMatrix();
    }
    
    FSceneConstant SceneConstants;
    SceneConstants.ViewMatrix = View;
    SceneConstants.ProjMatrix = Proj;
    USceneComponent* overrideComp = curEditorViewportClient->GetOverrideComponent();
    if (overrideComp)
    {
        SceneConstants.CameraPos = overrideComp->GetWorldLocation();
        SceneConstants.CameraLookAt = curEditorViewportClient->ViewTransformPerspective.GetLookAt();
    }
    else
    {
        SceneConstants.CameraPos = curEditorViewportClient->ViewTransformPerspective.GetLocation();
        SceneConstants.CameraLookAt = curEditorViewportClient->ViewTransformPerspective.GetLookAt();

    }

    renderResourceManager->UpdateConstantBuffer(TEXT("FSceneConstant"), &SceneConstants);
    
    for (const UBillboardComponent* item : BillboardComponents)
    {
        FDebugIconConstant DebugConstant;
        DebugConstant.IconPosition = item->GetWorldLocation();
        DebugConstant.IconScale = 0.2f; //TODO: 게임잼용 임시 스케일 변경

        renderResourceManager->UpdateConstantBuffer(TEXT("FDebugIconConstant"), &DebugConstant);

        Graphics.DeviceContext->PSSetShaderResources(0, 1, &item->Texture->TextureSRV);
        
        //TOD 이거 고쳐야함 Warning의 이유
        //D3D11 WARNING: ID3D11DeviceContext::Draw: Vertex Buffer at the input vertex slot 0 is not big enough for what the Draw*() call expects to traverse. This is OK, as reading off the end of the Buffer is defined to return 0. However the developer probably did not intend to make use of this behavior.  [ EXECUTION WARNING #356: DEVICE_DRAW_VERTEX_BUFFER_TOO_SMALL]
        Graphics.DeviceContext->Draw(6, 0); // 내부에서 버텍스 사용중
    }
}

void FEditorIconRenderPass::ClearRenderObjects()
{
    BillboardComponents.Empty();
}
