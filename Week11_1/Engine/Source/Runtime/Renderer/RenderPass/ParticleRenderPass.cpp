#include "ParticleRenderPass.h"

#include "EditorEngine.h"
#include "LaunchEngineLoop.h"
#include "ShowFlags.h"
#include "BaseGizmos/GizmoBaseComponent.h"
#include "D3D11RHI/CBStructDefine.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Renderer/Renderer.h"
#include "Renderer/VBIBTopologyMapping.h"
#include "UnrealEd/EditorViewportClient.h"
#include "UnrealEd/PrimitiveBatch.h"

#include "Components/LightComponents/DirectionalLightComponent.h"
#include "Components/LightComponents/PointLightComponent.h"
#include "Components/LightComponents/SpotLightComponent.h"
#include "Components/Material/Material.h"
#include "Components/Mesh/StaticMesh.h"
#include "Components/PrimitiveComponents/ParticleSystemComponent.h"
#include "Components/PrimitiveComponents/QuadTexture.h"
#include "Components/PrimitiveComponents/MeshComponents/StaticMeshComponents/SkySphereComponent.h"
#include "LevelEditor/SLevelEditor.h"
#include "Particles/ParticleHelper.h"
#include "Renderer/ComputeShader/ComputeTileLightCulling.h"

#include "UObject/UObjectIterator.h"
#include <Particles/ParticleMacros.h>

FParticleRenderPass::FParticleRenderPass(const FName& InShaderName) : FBaseRenderPass(InShaderName)
{
    const FGraphicsDevice& Graphics = GEngineLoop.GraphicDevice;

    D3D11_SAMPLER_DESC desc = {};
    desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    desc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;

    Graphics.Device->CreateSamplerState(&desc, &ShadowMapSampler);

    CreateDummyTexture();

    D3D11_BUFFER_DESC constdesc = {};
    constdesc.ByteWidth = sizeof(FLightingConstants);
    constdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constdesc.Usage = D3D11_USAGE_DYNAMIC;
    constdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    Graphics.Device->CreateBuffer(&constdesc, nullptr, &LightConstantBuffer);

    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    //TODO: SpriteParticle에 맞는걸로 넣어주기 지금은 하드코딩
    bufferDesc.ByteWidth = static_cast<UINT>(sizeof(FParticleSpriteVertex) * 512);
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = sizeof(FParticleSpriteVertex);

    Graphics.Device->CreateBuffer(&bufferDesc, nullptr, &SpriteParticleInstanceBuffer);
    
    CreateQuadTextureVertexBuffer();
}


void FParticleRenderPass::CreateQuadTextureVertexBuffer()
{
    FRenderer& Renderer = GEngineLoop.Renderer;
    FRenderResourceManager* ResourceManager = Renderer.GetResourceManager();
    
    if (ResourceManager->GetVertexBuffer(TEXT("QuadVB")) && ResourceManager->GetVertexBuffer(TEXT("QuadIB")))
    {
        return;
    }
    
    ID3D11Buffer* VB = ResourceManager->CreateImmutableVertexBuffer(quadTextureVertices, sizeof(quadTextureVertices));
    ResourceManager->AddOrSetVertexBuffer(TEXT("QuadVB"), VB);
    Renderer.MappingVBTopology(TEXT("Quad"), TEXT("QuadVB"), sizeof(FVertexTexture), 4);

    ID3D11Buffer* IB = ResourceManager->CreateIndexBuffer(quadTextureInices, sizeof(quadTextureInices) / sizeof(uint32));
    ResourceManager->AddOrSetIndexBuffer(TEXT("QuadIB"), IB);
    Renderer.MappingIB(TEXT("Quad"), TEXT("QuadIB"), sizeof(quadTextureInices) / sizeof(uint32));

}

void FParticleRenderPass::AddRenderObjectsToRenderPass(UWorld* World)
{
    for (USceneComponent* SceneComponent : TObjectRange<USceneComponent>())
    {
        if (SceneComponent->GetWorld() != World)
        {
            continue;
        }
                
        if (UParticleSystemComponent* ParticleSystemComponent = Cast<UParticleSystemComponent>(SceneComponent))
        {
            ParticleSystemComponents.Add(ParticleSystemComponent);
        }
            
        if (ULightComponentBase* LightComponent = Cast<ULightComponentBase>(SceneComponent))
        {
            LightComponents.Add(LightComponent);
        }
    }
}

FVector2D GetParticleSize(const FBaseParticle& Particle, const FDynamicSpriteEmitterReplayDataBase& Source)
{
    FVector2D Size;
    Size.X = FMath::Abs(Particle.Size.X * Source.Scale.X);
    Size.Y = FMath::Abs(Particle.Size.Y * Source.Scale.Y);
    
    return Size;
}

void FParticleRenderPass::Prepare(const std::shared_ptr<FViewportClient> InViewportClient)
{
    FBaseRenderPass::Prepare(InViewportClient);

    const FRenderer& Renderer = GEngineLoop.Renderer;
    const FGraphicsDevice& Graphics = GEngineLoop.GraphicDevice;

    Graphics.DeviceContext->OMSetDepthStencilState(Renderer.GetDepthStencilState(EDepthStencilState::LessEqual), 0);
    Graphics.DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 정점 연결 방식 설정
    Graphics.DeviceContext->RSSetState(Renderer.GetCurrentRasterizerState());

    // RTVs 배열의 유효성을 확인합니다.
    const auto CurRTV = Graphics.GetCurrentRenderTargetView();
    if (CurRTV != nullptr)
    {
        Graphics.DeviceContext->OMSetRenderTargets(1, &CurRTV, Graphics.GetCurrentWindowData()->DepthStencilView); // 렌더 타겟 설정
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

    ID3D11SamplerState* PostProcessSampler = Renderer.GetSamplerState(ESamplerType::PostProcess);
    Graphics.DeviceContext->PSSetSamplers(static_cast<uint32>(ESamplerType::PostProcess), 1, &PostProcessSampler);
}

//TODO: Test후 삭제
#include <random>

void FParticleRenderPass::Execute(const std::shared_ptr<FViewportClient> InViewportClient)
{
    FRenderer& Renderer = GEngineLoop.Renderer;
    FGraphicsDevice& Graphics = GEngineLoop.GraphicDevice;
    FRenderResourceManager* RenderResourceManager = Renderer.GetResourceManager();

    FMatrix View = FMatrix::Identity;
    FMatrix Proj = FMatrix::Identity;
    FMatrix InvView = FMatrix::Identity;
    
    std::shared_ptr<FEditorViewportClient> curEditorViewportClient = std::dynamic_pointer_cast<FEditorViewportClient>(InViewportClient);

    if (curEditorViewportClient != nullptr)
    {
        View = curEditorViewportClient->GetViewMatrix();
        Proj = curEditorViewportClient->GetProjectionMatrix();
        InvView = curEditorViewportClient->GetInvViewMatrix();
    }
    
    UpdateCameraConstant(InViewportClient);
    UpdateLightConstants();
    UpdateFlagConstant();

    FVector CameraLocation = curEditorViewportClient->ViewTransformPerspective.GetLocation();
    
    for (UParticleSystemComponent* ParticleSystemComponent : ParticleSystemComponents)
    {        
        for (FDynamicEmitterDataBase* ParticleRenderData : ParticleSystemComponent->EmitterRenderData)
        {   //EmitterRenderData에는 현존하는 파티클들이 담겨있음.
            // 렌더데이터 소팅            
            switch (ParticleRenderData->GetSource().eEmitterType)
            {
                case DET_Unknown:
                    {
                        break;
                    }
                case DET_Sprite:
                    { //일단 Sprite만 처리해
                        //Quad VIBuffer Bind
                        const std::shared_ptr<FVBIBTopologyMapping> VBIBTopMappingInfo = Renderer.GetVBIBTopologyMapping("Quad");
                        VBIBTopMappingInfo->Bind();

                        //TODO: 이거 제대로 캐스팅 되는지 확인해봐야함
                        const FDynamicSpriteEmitterReplayDataBase& Source = static_cast<const FDynamicSpriteEmitterReplayDataBase&>(ParticleRenderData->GetSource());

                        int32 VertexStride = sizeof(FParticleSpriteVertex);
                        int32 ParticleCount = Source.ActiveParticleCount;
                        // int32 VertexDynamicParameterStride = sizeof(FParticleVertexDynamicParameter);
                        TArray<std::pair<float, FParticleSpriteVertex>> ParticleVerticesWithDistance;

                        FTexture* Texture = nullptr;
                        
                        float SubImageIndex = 0.0f;
                        
                        const uint8* ParticleData = Source.DataContainer.ParticleData;
                        const uint16* ParticleIndices = Source.DataContainer.ParticleIndices;
                        // const FParticleOrder* OrderedIndices = ParticleOrder;

                        for (int i=0;i<ParticleCount;i++)
                        {
                            int32 ParticleIndex = i;
                            DECLARE_PARTICLE_CONST(Particle, ParticleData + Source.ParticleStride * ParticleIndices[ParticleIndex]);

                            const FVector2D Size = GetParticleSize(Particle, Source);
                            FVector ParticlePosition = Particle.Location;
                            FLinearColor ParticleColor = Particle.Color;

                            // if (Source.CameraPayloadOffset != 0)
                            // {
                            //     FVector CameraOffset = GetCameraOffsetFromPayload(Source.CameraPayloadOffset, Particle, ParticlePosition, CameraPosition);
                            //     ParticlePosition += CameraOffset;
                            // }

                            if (Source.SubUVDataOffset > 0)
                            {
                                FFullSubUVPayload* SubUVPayload = (FFullSubUVPayload*)(((uint8*)&Particle) + Source.SubUVDataOffset);
                                SubImageIndex = SubUVPayload->ImageIndex;
                            }

                            // if (Source.DynamicParameterDataOffset > 0)
                            // {
                            //     GetDynamicValueFromPayload(Source.DynamicParameterDataOffset, Particle, DynamicParameterValue);
                            // }

                            //서브인덱스 테스트
                            std::random_device rd;
                            std::mt19937 gen(rd()); // 시드 생성기
                            std::uniform_int_distribution<int> distInt(0, 3);
                            std::uniform_real_distribution<float> dist(0.0f, 1.0f);
                            SubImageIndex = distInt(gen);

                            ParticleColor = FLinearColor(dist(gen), dist(gen), dist(gen), 1);
                            
                            FParticleSpriteVertex FillVertex;
                            FillVertex.Position = ParticlePosition;
                            FillVertex.Size = FVector2D(GetParticleSizeWithUVFlipInSign(Particle, Size));
                            FillVertex.Rotation = Particle.Rotation;
                            FillVertex.SubImageIndex = SubImageIndex;
                            FillVertex.Color = ParticleColor;
                            // if (bUsesDynamicParameter)
                            // {
                            //     DynFillVertex = (FParticleVertexDynamicParameter*)TempDynamicParameterVert;
                            //     DynFillVertex->DynamicValue[0] = DynamicParameterValue.X;
                            //     DynFillVertex->DynamicValue[1] = DynamicParameterValue.Y;
                            //     DynFillVertex->DynamicValue[2] = DynamicParameterValue.Z;
                            //     DynFillVertex->DynamicValue[3] = DynamicParameterValue.W;
                            //     TempDynamicParameterVert += VertexDynamicParameterStride;
                            // }

                            //컴포넌트 기준 로컬 위치기 때문에 컴포넌트의 위치를 더해줘야함
                            FVector ParticleLocation = Particle.Location + ParticleSystemComponent->GetWorldLocation();
                            
                            float Distance = (ParticleLocation - CameraLocation).MagnitudeSquared();
                            
                            ParticleVerticesWithDistance.Add({Distance, FillVertex});

                            Texture = Source.Texture;
                        }

                        TArray<FParticleSpriteVertex> InstanceVertices;
                        ExtractDescSort(InstanceVertices, ParticleVerticesWithDistance);
                        
                        ID3D11InputLayout* ParticleInputLayout = Renderer.GetResourceManager()->GetInputLayout(TEXT("SpriteParticle"));
                        //Quad인 경우 Quad값 IASetVertexBuffer(0)에다 박고 1은 렌더데이터 돌면서 애들 다른애들마다 박아줘야함
                        UpdateInstanceBuffer<FParticleSpriteVertex>(Graphics.DeviceContext, SpriteParticleInstanceBuffer, ParticleInputLayout, InstanceVertices, VertexStride);
                        
                        //블렌드 설정 + Alpha값 주기
                        float blendFactor[4] = {0, 0, 0, 0};
                        ID3D11BlendState* AlphaBlend = RenderResourceManager->GetBlendState(EBlendState::AlphaBlend);
                        Graphics.DeviceContext->OMSetBlendState(AlphaBlend, blendFactor, 0xffffffff); // 블렌딩 상태 설정, 기본 블렌딩 상태임
                        
                        int TextureCountX = Source.SubImages_Horizontal;
                        int TextureCountY = Source.SubImages_Vertical;

                        //subuv테스트용
                        TextureCountX = 4;
                        TextureCountY = 1;
                        
                        UpdateTextureSizeConstants(TextureCountX, TextureCountY);
                        UpdateParticleConstants(0.5f);
                        //이거 Material로 바꾸면 아래걸로 변경
                        // UpdateMaterialConstants();
                        Graphics.DeviceContext->PSSetShaderResources(0, 1, &Texture->TextureSRV);
                        break;
                    }
                case DET_Mesh:
                    {
                        // SubSet마다 Material Update 및 Draw
                        // If There's No Material Subset
                        // if (ParticleRenderData->MaterialSubsets.Num() == 0)
                        // {
                        //     Graphics.DeviceContext->DrawIndexed(VBIBTopMappingInfo->GetNumIndices(), 0,0);
                        // }
                        // for (int subMeshIndex = 0; subMeshIndex < renderData->MaterialSubsets.Num(); ++subMeshIndex)
                        // {
                        //     const int materialIndex = renderData->MaterialSubsets[subMeshIndex].MaterialIndex;
                        //     
                        //     UpdateMaterialConstants(ParticleSystemComponent->GetMaterial(materialIndex)->GetMaterialInfo());
                        //
                        //     // index draw
                        //     const uint64 startIndex = renderData->MaterialSubsets[subMeshIndex].IndexStart;
                        //     const uint64 indexCount = renderData->MaterialSubsets[subMeshIndex].IndexCount;
                        // }
                        break;
                    }
                case DET_Beam2:
                    {
                        break;
                    }
                case DET_Ribbon:
                    {
                        break;
                    }
            default:
                break;
            }
            //ParticleComponent가 움직이면 Particle도 같이 움직여야하기 때문에 Model값 가져가야함
            UpdateMatrixConstants(ParticleSystemComponent, View, Proj, InvView);
            
            //일단 쿼드기준으로 그리기
            Graphics.DeviceContext->DrawIndexedInstanced(
                6,               // indexCount (쿼드 하나 = 2 tri = 6 index)
                ParticleRenderData->GetSource().ActiveParticleCount,   // instanceCount
                0,               // startIndexLocation
                0,               // baseVertexLocation
                0                // startInstanceLocation
            );
        }
    }

    ID3D11ShaderResourceView* nullSRVs[8] = { nullptr };
    ID3D11ShaderResourceView* nullSRV[4] = { nullptr };
    Graphics.DeviceContext->PSSetShaderResources(2, 1, &nullSRVs[0]); //쓰고 해제 나중에 이쁘게 뺴기
    Graphics.DeviceContext->PSSetShaderResources(3, 8, nullSRVs);
    Graphics.DeviceContext->PSSetShaderResources(11, 4, nullSRV);
    Graphics.DeviceContext->PSSetShaderResources(15, 8, nullSRVs);
    Graphics.DeviceContext->PSSetShaderResources(23, 8, nullSRVs);

    Graphics.DeviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff); // 블렌딩 상태 설정, 기본 블렌딩 상태임
}

void FParticleRenderPass::ExtractDescSort(TArray<FParticleSpriteVertex>& OutInstanceVertices, TArray<std::pair<float, FParticleSpriteVertex>>& ParticleVerticesWithDistance)
{
    std::sort(ParticleVerticesWithDistance.begin(), ParticleVerticesWithDistance.end(),
    [](const std::pair<float, FParticleSpriteVertex>& a, const std::pair<float, FParticleSpriteVertex>& b)
    {
        return a.first > b.first; // 내림차순
    });
                            
    for (std::pair<float, FParticleSpriteVertex> ParticleVertex : ParticleVerticesWithDistance)
    {
        OutInstanceVertices.Add(ParticleVertex.second);
    }
}

void FParticleRenderPass::CreateDummyTexture()
{
    FGraphicsDevice& Graphics = GEngineLoop.GraphicDevice;
    // 1x1 흰색 텍스처
    uint32_t whitePixel = 0xFFFFFFFF; // RGBA (1.0, 1.0, 1.0, 1.0)

    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width = 1;
    texDesc.Height = 1;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.Usage = D3D11_USAGE_IMMUTABLE;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    texDesc.SampleDesc.Count = 1;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = &whitePixel;
    initData.SysMemPitch = sizeof(uint32_t);

    ID3D11Texture2D* tex = nullptr;
    HRESULT hr = Graphics.Device->CreateTexture2D(&texDesc, &initData, &tex);
    if (FAILED(hr)) return;

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = texDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    hr = Graphics.Device->CreateShaderResourceView(tex, &srvDesc, &DummyWhiteTextureSRV);
    tex->Release(); // SRV에 참조 복사되므로 해제 OK
}

void FParticleRenderPass::ClearRenderObjects()
{
    ParticleSystemComponents.Empty();
    LightComponents.Empty();
}

void FParticleRenderPass::UpdateMatrixConstants(UParticleSystemComponent* InParticleSystemComponent, const FMatrix& InView, const FMatrix& InProjection, const FMatrix&
                                                 InInvView)
{
    FRenderResourceManager* renderResourceManager = GEngineLoop.Renderer.GetResourceManager();
    // MVP Update
    const FMatrix Model = InParticleSystemComponent->GetWorldMatrix();
    // const FMatrix NormalMatrix = FMatrix::Transpose(FMatrix::Inverse(Model));
    FMatrixSeparatedMVPConstants MatrixConstants;
    MatrixConstants.Model = Model;
    MatrixConstants.View = InView;
    MatrixConstants.Proj = InProjection;
    MatrixConstants.InvView = InInvView;
    
    renderResourceManager->UpdateConstantBuffer(TEXT("FMatrixSeparatedMVPConstants"), &MatrixConstants);
}

void FParticleRenderPass::UpdateParticleConstants(float InAlpha)
{
    FRenderResourceManager* renderResourceManager = GEngineLoop.Renderer.GetResourceManager();

    FParticleConstant ParticleConstant;
    ParticleConstant.Alpha = InAlpha;
    
    renderResourceManager->UpdateConstantBuffer(TEXT("FParticleConstant"), &ParticleConstant);
}

void FParticleRenderPass::UpdateTextureSizeConstants(int InCountX, int InCountY)
{
    FRenderResourceManager* renderResourceManager = GEngineLoop.Renderer.GetResourceManager();
    
    FTextureCountConstants TextureCountConstant;
    TextureCountConstant.CountX = InCountX;
    TextureCountConstant.CountY = InCountY;

    renderResourceManager->UpdateConstantBuffer(TEXT("FTextureCountConstants"), &TextureCountConstant);
}

void FParticleRenderPass::UpdateFlagConstant()
{
    FRenderResourceManager* renderResourceManager = GEngineLoop.Renderer.GetResourceManager();

    FFlagConstants FlagConstant;

    FlagConstant.IsLit = GEngineLoop.Renderer.bIsLit;
    FlagConstant.IsNormal = GEngineLoop.Renderer.bIsNormal;
    FlagConstant.IsVSM = GEngineLoop.Renderer.GetShadowFilterMode();

    renderResourceManager->UpdateConstantBuffer(TEXT("FFlagConstants"), &FlagConstant);
}

void FParticleRenderPass::UpdateLightConstants()
{
    UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
    if (EditorEngine == nullptr)
    {
        return;
    }
    
    FRenderResourceManager* renderResourceManager = GEngineLoop.Renderer.GetResourceManager();
    FGraphicsDevice& Graphics = GEngineLoop.GraphicDevice;

    FLightingConstants LightConstant;
    uint32 PointLightCount = 0;
    uint32 SpotLightCount = 0;

    FMatrix View = EditorEngine->GetLevelEditor()->GetActiveViewportClient()->GetViewMatrix();
    FMatrix Projection = EditorEngine->GetLevelEditor()->GetActiveViewportClient()->GetProjectionMatrix();
    FFrustum CameraFrustum = FFrustum::ExtractFrustum(View*Projection);
    ID3D11ShaderResourceView* ShadowMaps[8] = { nullptr };
    ID3D11ShaderResourceView* ShadowCubeMap[8] = { nullptr };
    for (ULightComponentBase* Comp : LightComponents)
    {
        if (!IsLightInFrustum(Comp, CameraFrustum))
        {
            continue;
        }

        if (const UPointLightComponent* PointLightComp = Cast<UPointLightComponent>(Comp))
        {
            if (PointLightCount > MAX_POINTLIGHT-1)
            {
                PointLightCount = MAX_POINTLIGHT-1;
                continue;
            }

            LightConstant.PointLights[PointLightCount].Color = PointLightComp->GetLightColor();
            LightConstant.PointLights[PointLightCount].Intensity = PointLightComp->GetIntensity();
            LightConstant.PointLights[PointLightCount].Position = PointLightComp->GetWorldLocation();
            LightConstant.PointLights[PointLightCount].Radius = PointLightComp->GetRadius();
            LightConstant.PointLights[PointLightCount].AttenuationFalloff = PointLightComp->GetAttenuationFalloff();
            LightConstant.PointLights[PointLightCount].CastShadow = PointLightComp->CanCastShadows();
;
            ShadowCubeMap[PointLightCount] = PointLightComp->GetShadowResource()->GetSRV();

            for (int face = 0;face < 6;face++)
            {
                LightConstant.PointLights[PointLightCount].PointLightView[face] = PointLightComp->GetViewMatrixForFace(face);
            }

            LightConstant.PointLights[PointLightCount].PointLightProj = PointLightComp->GetProjectionMatrix();

            PointLightCount++;
            continue;
        }
        
        if (const UDirectionalLightComponent* DirectionalLightComp = Cast<UDirectionalLightComponent>(Comp))
        {
            LightConstant.DirLight.Color = DirectionalLightComp->GetLightColor();
            LightConstant.DirLight.Intensity = DirectionalLightComp->GetIntensity();
            LightConstant.DirLight.Direction = DirectionalLightComp->GetWorldForwardVector();
            LightConstant.DirLight.CastShadow = DirectionalLightComp->CanCastShadows();

            TArray<ID3D11ShaderResourceView*> DirectionalShadowMaps;
            for (uint32 i = 0; i < CASCADE_COUNT; i++)
            {
                LightConstant.DirLight.View[i] = DirectionalLightComp->GetCascadeViewMatrix(i);
                LightConstant.DirLight.Projection[i] = DirectionalLightComp->GetCascadeProjectionMatrix(i);
                LightConstant.DirLight.CascadeSplit[i] = EditorEngine->GetLevelEditor()->GetActiveViewportClient()->GetCascadeSplit(i);
                ID3D11ShaderResourceView* DirectionalLightSRV = DirectionalLightComp->GetShadowResource()[i].GetSRV();
                //if (GEngineLoop.Renderer.GetShadowFilterMode() == EShadowFilterMode::VSM)
                //{
                //    DirectionalLightSRV = DirectionalLightComp->GetShadowResource()[i].GetVSMSRV();
                //}
                DirectionalShadowMaps.Add(DirectionalLightSRV);
            }
            Graphics.DeviceContext->PSSetShaderResources(11, 4, DirectionalShadowMaps.GetData());
            continue;
        }

        if (USpotLightComponent* SpotLightComp = Cast<USpotLightComponent>(Comp))
        {
            if (SpotLightCount > MAX_SPOTLIGHT-1) 
            {
                SpotLightCount = MAX_POINTLIGHT-1;
                continue;
            }

            LightConstant.SpotLights[SpotLightCount].Position = SpotLightComp->GetWorldLocation();
            LightConstant.SpotLights[SpotLightCount].Color = SpotLightComp->GetLightColor();
            LightConstant.SpotLights[SpotLightCount].Intensity = SpotLightComp->GetIntensity();
            LightConstant.SpotLights[SpotLightCount].Direction = SpotLightComp->GetWorldForwardVector();
            LightConstant.SpotLights[SpotLightCount].InnerAngle = SpotLightComp->GetInnerConeAngle();
            LightConstant.SpotLights[SpotLightCount].OuterAngle = SpotLightComp->GetOuterConeAngle();
            LightConstant.SpotLights[SpotLightCount].View = (SpotLightComp->GetViewMatrix());
            LightConstant.SpotLights[SpotLightCount].Proj = (SpotLightComp->GetProjectionMatrix());
            LightConstant.SpotLights[SpotLightCount].CastShadow = SpotLightComp->CanCastShadows();
            LightConstant.SpotLights[SpotLightCount].AtlasUVTransform = SpotLightComp->GetLightAtlasUV();
            ShadowMaps[SpotLightCount] = (GEngineLoop.Renderer.GetShadowFilterMode() == EShadowFilterMode::VSM) ? 
                SpotLightComp->GetShadowResource()->GetVSMSRV() : SpotLightComp->GetShadowResource()->GetSRV();
            SpotLightCount++;
            continue;
        }
    }

    // Binding ShadowMap
    for (int i = 0; i < 8; ++i)
    {
        if (ShadowMaps[i] == nullptr)
        {
            if (DummyWhiteTextureSRV == nullptr)
            {
                CreateDummyTexture();
            }
            ShadowMaps[i] = DummyWhiteTextureSRV;
        }
    }

    Graphics.DeviceContext->PSSetShaderResources(15, 8, ShadowCubeMap);
    //Graphics.DeviceContext->PSSetShaderResources(3, 8, ShadowMaps);
    // !NOTE : 아틀라스 텍스쳐는 이전 패스인 ShadowRenderPass에서 바인딩한다
    //UE_LOG(LogLevel::Error, "Point : %d, Spot : %d Dir : %d", PointLightCount, SpotLightCount, DirectionalLightCount);
    LightConstant.NumPointLights = PointLightCount;
    LightConstant.NumSpotLights = SpotLightCount;
    
    renderResourceManager->UpdateConstantBuffer(LightConstantBuffer, &LightConstant);
}

void FParticleRenderPass::UpdateMaterialConstants(const FObjMaterialInfo& MaterialInfo)
{
    FGraphicsDevice& Graphics = GEngineLoop.GraphicDevice;
    FRenderResourceManager* renderResourceManager = GEngineLoop.Renderer.GetResourceManager();
    
    FMaterialConstants MaterialConstants;
    MaterialConstants.DiffuseColor = MaterialInfo.Diffuse;
    MaterialConstants.TransparencyScalar = MaterialInfo.TransparencyScalar;
    MaterialConstants.MatAmbientColor = MaterialInfo.Ambient;
    MaterialConstants.DensityScalar = MaterialInfo.DensityScalar;
    MaterialConstants.SpecularColor = MaterialInfo.Specular;
    MaterialConstants.SpecularScalar = MaterialInfo.SpecularScalar;
    MaterialConstants.EmissiveColor = MaterialInfo.Emissive;
    //normalScale값 있는데 parse만 하고 constant로 넘기고 있진 않음
    MaterialConstants.bHasNormalTexture = false;
    
    if (MaterialInfo.bHasTexture == true)
    {
        const std::shared_ptr<FTexture> texture = GEngineLoop.ResourceManager.GetTexture(MaterialInfo.DiffuseTexturePath);
        const std::shared_ptr<FTexture> NormalTexture = GEngineLoop.ResourceManager.GetTexture(MaterialInfo.NormalTexturePath);
        if (texture)
        {
            Graphics.DeviceContext->PSSetShaderResources(0, 1, &texture->TextureSRV);
        }
        if (NormalTexture)
        {
            Graphics.DeviceContext->PSSetShaderResources(1, 1, &NormalTexture->TextureSRV);
            MaterialConstants.bHasNormalTexture = true;
        }
        
        ID3D11SamplerState* linearSampler = renderResourceManager->GetSamplerState(ESamplerType::Linear);
        Graphics.DeviceContext->PSSetSamplers(static_cast<uint32>(ESamplerType::Linear), 1, &linearSampler);
    }
    else
    {
        ID3D11ShaderResourceView* nullSRV[1] = {nullptr};
        Graphics.DeviceContext->PSSetShaderResources(0, 1, nullSRV);
    }
    renderResourceManager->UpdateConstantBuffer(renderResourceManager->GetConstantBuffer(TEXT("FMaterialConstants")), &MaterialConstants);
}

void FParticleRenderPass::UpdateCameraConstant(const std::shared_ptr<FViewportClient>& InViewportClient)
{
    const FGraphicsDevice& Graphics = GEngineLoop.GraphicDevice;
    FRenderResourceManager* renderResourceManager = GEngineLoop.Renderer.GetResourceManager();
    const std::shared_ptr<FEditorViewportClient> curEditorViewportClient = std::dynamic_pointer_cast<FEditorViewportClient>(InViewportClient);

    FCameraConstant CameraConstants;
    CameraConstants.CameraForward = FVector::ZeroVector;
    CameraConstants.CameraPos = curEditorViewportClient->ViewTransformPerspective.GetLocation();
    CameraConstants.ViewProjMatrix = FMatrix::Identity;
    CameraConstants.ProjMatrix = FMatrix::Identity;
    CameraConstants.ViewMatrix = FMatrix::Identity;
    CameraConstants.NearPlane = curEditorViewportClient->GetNearClip();
    CameraConstants.FarPlane = curEditorViewportClient->GetFarClip();

    renderResourceManager->UpdateConstantBuffer(renderResourceManager->GetConstantBuffer(TEXT("FCameraConstant")), &CameraConstants);
}

bool FParticleRenderPass::IsLightInFrustum(ULightComponentBase* LightComponent, const FFrustum& CameraFrustum) const
{
    // if (dynamic_cast<UDirectionalLightComponent*>(LightComponent) && !dynamic_cast<USpotLightComponent>(LightComponent))
    // {
    //     return true;
    // }

    // 포인트 라이트 : 구 형태 판단
    if (UPointLightComponent* PointLightComp = Cast<UPointLightComponent>(LightComponent))
    {
        FVector LightCenter = PointLightComp->GetWorldLocation();
        float Radius = PointLightComp->GetRadius();
        return CameraFrustum.IntersectsSphere(LightCenter, Radius);
    }

    // 스팟 라이트의 경우, 보통 구 또는 원뿔의 바운딩 볼륨을 사용합니다.
    if (USpotLightComponent* SpotLightComp = Cast<USpotLightComponent>(LightComponent))
    {
        // FVector LightCenter = SpotLightComp->GetComponentLocation();
        // // 스팟 라이트의 영향을 대략적으로 표현하는 반지름 (필요 시 실제 cone 계산으로 대체)
        // float ApproxRadius = SpotLightComp->GetOuterConeAngle(); // 예시: cone 각도를 사용 (단위 및 스케일은 조정 필요)
        // return CameraFrustum.IntersectsSphere(LightCenter, ApproxRadius);
        return IsSpotLightInFrustum(SpotLightComp, CameraFrustum);
    }
    
    // 그 외의 경우, 보수적으로 true로 반환
    return true;
}

bool FParticleRenderPass::IsSpotLightInFrustum(USpotLightComponent* SpotLightComp, const FFrustum& CameraFrustum) const
{
    // 스팟 라이트의 Apex(위치)
    FVector Apex = SpotLightComp->GetWorldLocation();

    // 스팟 라이트의 방향: 스팟 라이트의 오너의 전방벡터를 사용 (정규화된 값)
    FVector Dir = SpotLightComp->GetOwner()->GetActorForwardVector().Normalize();

    // 스팟 라이트의 범위 (거리) - 일반적으로 Attenuation Radius 또는 Range를 사용
    float Range = 50;

    // 스팟 라이트의 외부 콘 각도 (단위: 도)를 라디안으로 변환
    float OuterAngleRad = SpotLightComp->GetOuterConeAngle();

    // 원뿔의 베이스(밑면) 중심과 반지름 계산
    FVector BaseCenter = Apex + Dir * Range;
    float BaseRadius = Range * FMath::Tan(OuterAngleRad);

    // 1. Apex(꼭짓점)가 프러스텀 내부에 있으면 전체 원뿔도 영향을 줄 가능성이 높으므로 true
    if (CameraFrustum.IntersectsPoint(Apex))
    {
        return true;
    }

    // 2. 베이스 중심이 프러스텀 내부에 있으면 true
    if (CameraFrustum.IntersectsPoint(BaseCenter))
    {
        return true;
    }

    // 3. 베이스 원의 둘레를 여러 샘플링하여 프러스텀 내부 포함 여부 검사
    //    (정확도를 높이기 위해 샘플 수를 늘릴 수 있습니다)
    const int SampleCount = 8;  // 예제에서는 8개의 점으로 샘플링
    // 원뿔 베이스의 평면에 대한 임의의 좌표계를 생성
    FVector Right = Dir.Cross(FVector(0, 1, 0));
    if (Right.IsNearlyZero())  // 만약 Dir이 (0,1,0)와 평행하면 다른 벡터로 교차
    {
        Right = Dir.Cross(FVector(1, 0, 0));
    }
    Right.Normalize();
    FVector Up = Dir.Cross(Right).Normalize();

    for (int i = 0; i < SampleCount; ++i)
    {
        float Angle = (2.f * PI * i) / SampleCount;
        // 베이스 원의 둘레 상의 샘플 포인트 계산
        FVector Offset = (Right * FMath::Cos(Angle) + Up * FMath::Sin(Angle)) * BaseRadius;
        FVector SamplePoint = BaseCenter + Offset;

        // 샘플 포인트가 프러스텀 내부에 있으면 스팟 라이트 영향 영역이 프러스텀에 일부 포함된 것으로 판단
        if (CameraFrustum.IntersectsPoint(SamplePoint))
        {
            return true;
        }
    }
    
    // 모든 검사에서 프러스텀 내부에 포함된 점이 없으면 false
    return false;
}

