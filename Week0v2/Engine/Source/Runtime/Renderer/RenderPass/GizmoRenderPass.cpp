#include "GizmoRenderPass.h"
#include "EditorEngine.h"
#include "Actors/Player.h"
#include "BaseGizmos/GizmoBaseComponent.h"
#include "Components/SkySphereComponent.h"
#include "D3D11RHI/CBStructDefine.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Math/JungleMath.h"
#include "Renderer/Renderer.h"
#include "Renderer/VBIBTopologyMapping.h"
#include "UnrealEd/EditorViewportClient.h"
#include "UObject/ObjectTypes.h"
#include "UObject/UObjectIterator.h"


extern UEditorEngine* GEngine;

void FGizmoRenderPass::AddRenderObjectsToRenderPass(UWorld* InWorld)
{
    GizmoComponents.Empty();
    
    if (GEngine->GetWorld()->WorldType != EWorldType::Editor)
    {
        return;
    }
    
    if (InWorld->WorldType == EWorldType::Editor)
    {
        for (const auto iter : TObjectRange<USceneComponent>())
        {
            if (UGizmoBaseComponent* pGizmoComp = Cast<UGizmoBaseComponent>(iter))
            {
                GizmoComponents.Add(pGizmoComp);
            }
        }
    }
}

void FGizmoRenderPass::Prepare(const std::shared_ptr<FViewportClient> InViewportClient)
{
    FBaseRenderPass::Prepare(InViewportClient);

    const FRenderer& Renderer = GEngine->renderer;
    const FGraphicsDevice& Graphics = GEngine->graphicDevice;
    
    Graphics.DeviceContext->OMSetDepthStencilState(Renderer.GetDepthStencilState(EDepthStencilState::DepthNone), 0);
    Graphics.DeviceContext->RSSetState(Renderer.GetResourceManager()->GetRasterizerState(ERasterizerState::SolidBack));

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

void FGizmoRenderPass::Execute(const std::shared_ptr<FViewportClient> InViewportClient)
{
    FMatrix View = FMatrix::Identity;
    FMatrix Proj = FMatrix::Identity;

    FGraphicsDevice& Graphics = GEngine->graphicDevice;
    FRenderer& Renderer = GEngine->renderer;
    
    std::shared_ptr<FEditorViewportClient> curEditorViewportClient = std::dynamic_pointer_cast<FEditorViewportClient>(InViewportClient);
    if (curEditorViewportClient != nullptr)
    {
        View = curEditorViewportClient->GetViewMatrix();
        Proj = curEditorViewportClient->GetProjectionMatrix();
    }

    UWorld* World = GEngine->GetWorld();
    for (auto GizmoComp : GizmoComponents)
    {
        if ((GizmoComp->GetGizmoType() == UGizmoBaseComponent::ArrowX ||
            GizmoComp->GetGizmoType() == UGizmoBaseComponent::ArrowY ||
            GizmoComp->GetGizmoType() == UGizmoBaseComponent::ArrowZ)
            && World->GetEditorPlayer()->GetControlMode() != CM_TRANSLATION)
            continue;
        else if ((GizmoComp->GetGizmoType() == UGizmoBaseComponent::ScaleX ||
            GizmoComp->GetGizmoType() == UGizmoBaseComponent::ScaleY ||
            GizmoComp->GetGizmoType() == UGizmoBaseComponent::ScaleZ)
            && World->GetEditorPlayer()->GetControlMode() != CM_SCALE)
            continue;
        else if ((GizmoComp->GetGizmoType() == UGizmoBaseComponent::CircleX ||
            GizmoComp->GetGizmoType() == UGizmoBaseComponent::CircleY ||
            GizmoComp->GetGizmoType() == UGizmoBaseComponent::CircleZ)
            && World->GetEditorPlayer()->GetControlMode() != CM_ROTATION)
            continue;
        
        std::shared_ptr<FEditorViewportClient> CurrentEditorViewportClient = std::dynamic_pointer_cast<FEditorViewportClient>(InViewportClient);
        
        UpdateMatrixConstants(GizmoComp, View, Proj);

        if (!GizmoComp->GetStaticMesh()) continue;

        OBJ::FStaticMeshRenderData* renderData = GizmoComp->GetStaticMesh()->GetRenderData();
        if (renderData == nullptr) continue;

        // VIBuffer Bind
        const std::shared_ptr<FVBIBTopologyMapping> VBIBTopMappingInfo = Renderer.GetVBIBTopologyMapping(GizmoComp->GetVBIBTopologyMappingName());
        VBIBTopMappingInfo->Bind();

        // If There's No Material Subset
        if (renderData->MaterialSubsets.Num() == 0)
        {
            Graphics.DeviceContext->DrawIndexed(VBIBTopMappingInfo->GetNumIndices(), 0,0);
        }

        // SubSet마다 Material Update 및 Draw
        for (int subMeshIndex = 0; subMeshIndex < renderData->MaterialSubsets.Num(); ++subMeshIndex)
        {
            const int materialIndex = renderData->MaterialSubsets[subMeshIndex].MaterialIndex;
            
            UpdateMaterialConstants(GizmoComp->GetMaterial(materialIndex)->GetMaterialInfo());

            // index draw
            const uint64 startIndex = renderData->MaterialSubsets[subMeshIndex].IndexStart;
            const uint64 indexCount = renderData->MaterialSubsets[subMeshIndex].IndexCount;
            Graphics.DeviceContext->DrawIndexed(indexCount, startIndex, 0);
        }
    }
}

void FGizmoRenderPass::UpdateMatrixConstants(UGizmoBaseComponent* InGizmoComponent, const FMatrix& InView, const FMatrix& InProjection)
{
    FRenderResourceManager* renderResourceManager = GEngine->renderer.GetResourceManager();
    // MVP Update
    const FMatrix Model = JungleMath::CreateModelMatrix(InGizmoComponent->GetComponentLocation(), InGizmoComponent->GetComponentRotation(),
                                                        InGizmoComponent->GetComponentScale());
    const FMatrix NormalMatrix = FMatrix::Transpose(FMatrix::Inverse(Model));
        
    FMatrixConstants MatrixConstants;
    MatrixConstants.Model = Model;
    MatrixConstants.ViewProj = InView * InProjection;
    MatrixConstants.MInverseTranspose = NormalMatrix;
    if (InGizmoComponent->GetWorld()->GetSelectedActor() == InGizmoComponent->GetOwner())
    {
        MatrixConstants.isSelected = true;
    }
    else
    {
        MatrixConstants.isSelected = false;
    }
    renderResourceManager->UpdateConstantBuffer(renderResourceManager->GetConstantBuffer(TEXT("FMatrixConstants")), &MatrixConstants);
}

void FGizmoRenderPass::UpdateMaterialConstants(const FObjMaterialInfo& MaterialInfo)
{
    FGraphicsDevice& Graphics = GEngine->graphicDevice;
    FRenderResourceManager* renderResourceManager = GEngine->renderer.GetResourceManager();
    
    FMaterialConstants MaterialConstants;
    MaterialConstants.DiffuseColor = MaterialInfo.Diffuse;
    MaterialConstants.TransparencyScalar = MaterialInfo.TransparencyScalar;
    MaterialConstants.MatAmbientColor = MaterialInfo.Ambient;
    MaterialConstants.DensityScalar = MaterialInfo.DensityScalar;
    MaterialConstants.SpecularColor = MaterialInfo.Specular;
    MaterialConstants.SpecularScalar = MaterialInfo.SpecularScalar;
    MaterialConstants.EmissiveColor = MaterialInfo.Emissive;
    renderResourceManager->UpdateConstantBuffer(renderResourceManager->GetConstantBuffer(TEXT("FMaterialConstants")), &MaterialConstants);
    
    if (MaterialInfo.bHasTexture == true)
    {
        const std::shared_ptr<FTexture> texture = GEngine->resourceMgr.GetTexture(MaterialInfo.DiffuseTexturePath);
        Graphics.DeviceContext->PSSetShaderResources(0, 1, &texture->TextureSRV);
        ID3D11SamplerState* linearSampler = renderResourceManager->GetSamplerState(ESamplerType::Linear);
        Graphics.DeviceContext->PSSetSamplers(static_cast<uint32>(ESamplerType::Linear), 1, &linearSampler);
    }
    else
    {
        ID3D11ShaderResourceView* nullSRV[1] = {nullptr};
        Graphics.DeviceContext->PSSetShaderResources(0, 1, nullSRV);
    }
}
