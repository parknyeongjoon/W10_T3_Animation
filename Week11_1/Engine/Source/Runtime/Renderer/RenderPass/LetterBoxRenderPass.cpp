#include "LetterBoxRenderPass.h"

#include "PlayerCameraManager.h"
#include "EditorEngine.h"
#include "LaunchEngineLoop.h"
#include "D3D11RHI/CBStructDefine.h"
#include "D3D11RHI/GraphicDevice.h"
#include "Engine/World.h"
#include "Renderer/Renderer.h"
#include "UObject/UObjectIterator.h"

extern UEngine* GEngine;

FLetterBoxRenderPass::FLetterBoxRenderPass(const FName& InShaderName)
    : FBaseRenderPass(InShaderName)
{

    FRenderer& Renderer = GEngineLoop.Renderer;
    FRenderResourceManager* RenderResourceManager = Renderer.GetResourceManager();

    LetterBoxConstantBuffer = RenderResourceManager->CreateConstantBuffer(sizeof(FLetterBoxConstants));
}

void FLetterBoxRenderPass::AddRenderObjectsToRenderPass(UWorld* World)
{
    for (APlayerCameraManager* PlayerCameraManager : TObjectRange<APlayerCameraManager>())
    {
        if (PlayerCameraManager->GetWorld() != World)
        {
            continue;
        }
        PlayerCameraManagers.Add(PlayerCameraManager);
    }
}

void FLetterBoxRenderPass::Prepare(std::shared_ptr<FViewportClient> InViewportClient)
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
        
        ID3D11SamplerState* Sampler = Renderer.GetSamplerState(ESamplerType::Linear);
        Graphics.DeviceContext->PSSetSamplers(0, 1, &Sampler);

        const auto PreviousSRV = Graphics.GetPreviousShaderResourceView();
        Graphics.DeviceContext->PSSetShaderResources(1, 1, &PreviousSRV);
    }
}

void FLetterBoxRenderPass::Execute(const std::shared_ptr<FViewportClient> InViewportClient)
{
    if (bRender)
    {
        UpdateLetterConstant();
        UpdateScreenConstant(InViewportClient);
        
        FGraphicsDevice& Graphics = GEngineLoop.GraphicDevice;
        
        Graphics.DeviceContext->Draw(6, 0);

        bRender = false;
    }
}

void FLetterBoxRenderPass::ClearRenderObjects()
{
    PlayerCameraManagers.Empty();
}

void FLetterBoxRenderPass::UpdateLetterConstant()
{

    //레터박스는 마지막 하나만 적용
    
    FLetterBoxConstants LetterBoxConstants;
    FLinearColor FinalColor = {0.f,0.f,0.f,0.f}; // 시작은 완전 투명 (알파 0)
    float AccumulatedFadeAlpha = 0.0f;
    
    for (APlayerCameraManager* iter : PlayerCameraManagers)
    {
        const TArray<FPostProcessSettings>* PPSettingsPtr = nullptr;
        const TArray<float>* BlendWeightsPtr = nullptr;
        iter->GetCachedPostProcessBlends(PPSettingsPtr, BlendWeightsPtr);
        
        
        if (PPSettingsPtr )
        {
            int32 NumSettings = PPSettingsPtr->Num();
            for (int i = 0 ; i < NumSettings; ++i)
            {
                // 현재 레이어(i)의 설정과 가중치 가져오기
                const FPostProcessSettings& LayerSettings = (*PPSettingsPtr)[i];
                const float LayerWeight = 1.0f; // 이 모디파이어/설정의 전체 영향력

                LetterBoxConstants.LetterboxSize = LayerSettings.LetterboxSize;
                LetterBoxConstants.PillarboxSize = LayerSettings.PillarboxSize;

            }
        }
    }
    //
    // FadeConstants.FadeColor = FinalColor;
    // FadeConstants.FadeColor.A = 1.0f; // 알파는 1로 고정 (페이드 색상은 완전 불투명)
    // FadeConstants.FadeAlpha = AccumulatedFadeAlpha;
    //
    // if (FadeConstants.FadeAlpha >0.1f)
    // {
    //     int a = 0;
    // }


    const FGraphicsDevice& Graphics = GEngineLoop.GraphicDevice;
    FRenderResourceManager* renderResourceManager = GEngineLoop.Renderer.GetResourceManager();

    renderResourceManager->UpdateConstantBuffer(LetterBoxConstantBuffer, &LetterBoxConstants);
    Graphics.DeviceContext->PSSetConstantBuffers(0, 1, &LetterBoxConstantBuffer);

        
}
