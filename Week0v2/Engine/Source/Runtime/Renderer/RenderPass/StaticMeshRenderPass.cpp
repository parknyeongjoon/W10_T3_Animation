#include "StaticMeshRenderPass.h"

#include "EditorEngine.h"
#include "BaseGizmos/GizmoBaseComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/LightComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SkySphereComponent.h"
#include "D3D11RHI/CBStructDefine.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Math/JungleMath.h"
#include "PropertyEditor/ShowFlags.h"
#include "Renderer/Renderer.h"
#include "Renderer/VBIBTopologyMapping.h"
#include "UnrealEd/EditorViewportClient.h"
#include "UnrealEd/PrimitiveBatch.h"
#include "UObject/UObjectIterator.h"
#include <Components/SpotLightComponent.h>

extern UEditorEngine* GEngine;

void FStaticMeshRenderPass::AddRenderObjectsToRenderPass(UWorld* InWorld)
{
    StaticMesheComponents.Empty();

    LightComponents.Empty();
    
    if (InWorld->WorldType == EWorldType::Editor)
    {
        for (const auto iter : TObjectRange<USceneComponent>())
        {
            if (ULightComponentBase* pGizmoComp = Cast<ULightComponentBase>(iter))
            {
                LightComponents.Add(pGizmoComp);
            }
        }
    }
    
    for (const AActor* actor : InWorld->GetActors())
    {
        for (const UActorComponent* actorComp : actor->GetComponents())
        {
            if (UStaticMeshComponent* pStaticMeshComp = Cast<UStaticMeshComponent>(actorComp))
            {
                if (!Cast<UGizmoBaseComponent>(actorComp))
                    StaticMesheComponents.Add(pStaticMeshComp);
            }
        }
    }
}

void FStaticMeshRenderPass::Prepare(const std::shared_ptr<FViewportClient> InViewportClient)
{
    FBaseRenderPass::Prepare(InViewportClient);

    const FRenderer& Renderer = GEngine->renderer;
    const FGraphicsDevice& Graphics = GEngine->graphicDevice;

    Graphics.DeviceContext->OMSetDepthStencilState(Renderer.GetDepthStencilState(EDepthStencilState::LessEqual), 0);
    // Graphics.DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 정정 연결 방식 설정
    Graphics.DeviceContext->RSSetState(Renderer.GetCurrentRasterizerState());

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

void FStaticMeshRenderPass::Execute(const std::shared_ptr<FViewportClient> InViewportClient)
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
    
    for (UStaticMeshComponent* staticMeshComp : StaticMesheComponents)
    {
        const FMatrix Model = JungleMath::CreateModelMatrix(staticMeshComp->GetComponentLocation(), staticMeshComp->GetComponentRotation(),
                                                    staticMeshComp->GetComponentScale());
        
        UpdateMatrixConstants(staticMeshComp, View, Proj);

        // UpdateSkySphereTextureConstants(Cast<USkySphereComponent>(staticMeshComp));

        UpdateLightConstants();

        UpdateFlagConstant();
        
        if (curEditorViewportClient->GetShowFlag() & static_cast<uint64>(EEngineShowFlags::Type::SF_AABB))
        {
            if (GEngine->GetWorld()->GetSelectedActor() == staticMeshComp->GetOwner())
            {
                UPrimitiveBatch::GetInstance().AddAABB(
                    staticMeshComp->GetBoundingBox(),
                    staticMeshComp->GetComponentLocation(),
                    Model
                );
            }
        }

        if (!staticMeshComp->GetStaticMesh()) continue;
        
        const OBJ::FStaticMeshRenderData* renderData = staticMeshComp->GetStaticMesh()->GetRenderData();
        if (renderData == nullptr) continue;

        // VIBuffer Bind
        const std::shared_ptr<FVBIBTopologyMapping> VBIBTopMappingInfo = Renderer.GetVBIBTopologyMapping(staticMeshComp->GetVBIBTopologyMappingName());
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
            
            UpdateMaterialConstants(staticMeshComp->GetMaterial(materialIndex)->GetMaterialInfo());

            // index draw
            const uint64 startIndex = renderData->MaterialSubsets[subMeshIndex].IndexStart;
            const uint64 indexCount = renderData->MaterialSubsets[subMeshIndex].IndexCount;
            Graphics.DeviceContext->DrawIndexed(indexCount, startIndex, 0);
        }
    } 
}

void FStaticMeshRenderPass::UpdateMatrixConstants(UStaticMeshComponent* InStaticMeshComponent, const FMatrix& InView, const FMatrix& InProjection)
{
    FRenderResourceManager* renderResourceManager = GEngine->renderer.GetResourceManager();
    // MVP Update
    const FMatrix Model = JungleMath::CreateModelMatrix(InStaticMeshComponent->GetComponentLocation(), InStaticMeshComponent->GetComponentRotation(),
                                                        InStaticMeshComponent->GetComponentScale());
    const FMatrix NormalMatrix = FMatrix::Transpose(FMatrix::Inverse(Model));
        
    FMatrixConstants MatrixConstants;
    MatrixConstants.Model = Model;
    MatrixConstants.ViewProj = InView * InProjection;
    MatrixConstants.MInverseTranspose = NormalMatrix;
    if (InStaticMeshComponent->GetWorld()->GetSelectedActor() == InStaticMeshComponent->GetOwner())
    {
        MatrixConstants.isSelected = true;
    }
    else
    {
        MatrixConstants.isSelected = false;
    }
    renderResourceManager->UpdateConstantBuffer(renderResourceManager->GetConstantBuffer(TEXT("FMatrixConstants")), &MatrixConstants);
}

void FStaticMeshRenderPass::UpdateFlagConstant()
{
    FRenderResourceManager* renderResourceManager = GEngine->renderer.GetResourceManager();

    FFlagConstants FlagConstant;

    FlagConstant.IsLit = GEngine->renderer.bIsLit;

    renderResourceManager->UpdateConstantBuffer(renderResourceManager->GetConstantBuffer(TEXT("FFlagConstants")), &FlagConstant);
}

void FStaticMeshRenderPass::UpdateLightConstants()
{
    FRenderResourceManager* renderResourceManager = GEngine->renderer.GetResourceManager();

    FLightingConstants LightConstant;
    uint32 DirectionalLightCount = 0;
    uint32 PointLightCount = 0;
    uint32 SpotLightCount = 0;

    for (ULightComponentBase* Comp : LightComponents)
    {
        UPointLightComponent* PointLightComp = dynamic_cast<UPointLightComponent*>(Comp);

        if (PointLightComp)
        {
            LightConstant.PointLights[PointLightCount].Color = PointLightComp->GetColor();
            LightConstant.PointLights[PointLightCount].Intensity = PointLightComp->GetIntensity();
            LightConstant.PointLights[PointLightCount].Position = PointLightComp->GetComponentLocation();
            LightConstant.PointLights[PointLightCount].Radius = PointLightComp->GetRadius();
            LightConstant.PointLights[PointLightCount].AttenuationFalloff = PointLightComp->GetAttenuationFalloff();
            PointLightCount++;
            continue;
        }

        UDirectionalLightComponent* DirectionalLightComp = dynamic_cast<UDirectionalLightComponent*>(Comp);
        if (DirectionalLightComp)
        {
            USpotLightComponent* SpotLightComp = Cast<USpotLightComponent>(DirectionalLightComp);
            if (SpotLightComp)
            {
                LightConstant.SpotLights[SpotLightCount].Position = SpotLightComp->GetComponentLocation();
                LightConstant.SpotLights[SpotLightCount].Color = SpotLightComp->GetColor();
                LightConstant.SpotLights[SpotLightCount].Intensity = SpotLightComp->GetIntensity();
                LightConstant.SpotLights[SpotLightCount].Direction = SpotLightComp->GetOwner()->GetActorForwardVector();
                LightConstant.SpotLights[SpotLightCount].InnerAngle = SpotLightComp->GetInnerConeAngle();
                LightConstant.SpotLights[SpotLightCount].OuterAngle = SpotLightComp->GetOuterConeAngle();
                SpotLightCount++;
                continue;
            }
            LightConstant.DirLights[DirectionalLightCount].Color = DirectionalLightComp->GetColor();
            LightConstant.DirLights[DirectionalLightCount].Intensity = DirectionalLightComp->GetIntensity();
            LightConstant.DirLights[DirectionalLightCount].Direction = DirectionalLightComp->GetForwardVector();
            DirectionalLightCount++;
            continue;
        }
    }

    LightConstant.NumPointLights = PointLightCount;
    LightConstant.NumDirectionalLights = DirectionalLightCount;
    LightConstant.NumSpotLights = SpotLightCount;
    
    renderResourceManager->UpdateConstantBuffer(renderResourceManager->GetConstantBuffer(TEXT("FLightingConstants")), &LightConstant);
}

void FStaticMeshRenderPass::UpdateSkySphereTextureConstants(const USkySphereComponent* InSkySphereComponent)
{
    FRenderResourceManager* renderResourceManager = GEngine->renderer.GetResourceManager();
    
    FSubUVConstant UVBuffer;
    
    if (InSkySphereComponent != nullptr)
    {
        UVBuffer.indexU = InSkySphereComponent->UOffset;
        UVBuffer.indexV = InSkySphereComponent->VOffset;
    }
    else
    {
        UVBuffer.indexU = 0;
        UVBuffer.indexV = 0;
    }
    
    renderResourceManager->UpdateConstantBuffer(renderResourceManager->GetConstantBuffer(TEXT("FSubUVConstant")), &UVBuffer);
}

void FStaticMeshRenderPass::UpdateMaterialConstants(const FObjMaterialInfo& MaterialInfo)
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
    //normalScale값 있는데 parse만 하고 constant로 넘기고 있진 않음
    renderResourceManager->UpdateConstantBuffer(renderResourceManager->GetConstantBuffer(TEXT("FMaterialConstants")), &MaterialConstants);
    
    if (MaterialInfo.bHasTexture == true)
    {
        const std::shared_ptr<FTexture> texture = GEngine->resourceMgr.GetTexture(MaterialInfo.DiffuseTexturePath);
        const std::shared_ptr<FTexture> NormalTexture = GEngine->resourceMgr.GetTexture(MaterialInfo.NormalTexturePath);
        if (texture)
        {
            Graphics.DeviceContext->PSSetShaderResources(0, 1, &texture->TextureSRV);
        }
        if (NormalTexture)
        {
            Graphics.DeviceContext->PSSetShaderResources(1, 1, &NormalTexture->TextureSRV);
        }
        
        ID3D11SamplerState* linearSampler = renderResourceManager->GetSamplerState(ESamplerType::Linear);
        Graphics.DeviceContext->PSSetSamplers(static_cast<uint32>(ESamplerType::Linear), 1, &linearSampler);
    }
    else
    {
        ID3D11ShaderResourceView* nullSRV[1] = {nullptr};
        Graphics.DeviceContext->PSSetShaderResources(0, 1, nullSRV);
    }
}
