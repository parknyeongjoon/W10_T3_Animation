#include "StaticMeshRenderPass.h"

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
#include "Components/PrimitiveComponents/MeshComponents/StaticMeshComponents/SkySphereComponent.h"
#include "LevelEditor/SLevelEditor.h"
#include "Renderer/ComputeShader/ComputeTileLightCulling.h"

#include "UObject/UObjectIterator.h"

extern UEngine* GEngine;

FStaticMeshRenderPass::FStaticMeshRenderPass(const FName& InShaderName) : FBaseRenderPass(InShaderName)
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
}

void FStaticMeshRenderPass::AddRenderObjectsToRenderPass(UWorld* World)
{
    for (USceneComponent* SceneComponent : TObjectRange<USceneComponent>())
    {
        if (SceneComponent->GetWorld() != World)
        {
            continue;
        }
                
        if (UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(SceneComponent))
        {
            if (!Cast<UGizmoBaseComponent>(StaticMeshComponent))
            {
                StaticMeshComponents.Add(StaticMeshComponent);
            }
        }
            
        if (ULightComponentBase* LightComponent = Cast<ULightComponentBase>(SceneComponent))
        {
            LightComponents.Add(LightComponent);
        }
    }
}

void FStaticMeshRenderPass::Prepare(const std::shared_ptr<FViewportClient> InViewportClient)
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

void FStaticMeshRenderPass::UpdateComputeResource()
{
    FRenderer& Renderer = GEngineLoop.Renderer;
    FGraphicsDevice& Graphics = GEngineLoop.GraphicDevice;
    FRenderResourceManager* renderResourceManager = Renderer.GetResourceManager(); 

    ID3D11ShaderResourceView* TileCullingSRV = renderResourceManager->GetStructuredBufferSRV("TileLightCulling");

    Graphics.DeviceContext->CSSetShader(nullptr, nullptr, 0);
    
    Graphics.DeviceContext->PSSetShaderResources(2, 1, &TileCullingSRV);
}

void FStaticMeshRenderPass::Execute(const std::shared_ptr<FViewportClient> InViewportClient)
{
    FRenderer& Renderer = GEngineLoop.Renderer;
    FGraphicsDevice& Graphics = GEngineLoop.GraphicDevice;
    
    FMatrix View = FMatrix::Identity;
    FMatrix Proj = FMatrix::Identity;

    std::shared_ptr<FEditorViewportClient> curEditorViewportClient = std::dynamic_pointer_cast<FEditorViewportClient>(InViewportClient);
    if (curEditorViewportClient != nullptr)
    {
        View = curEditorViewportClient->GetViewMatrix();
        Proj = curEditorViewportClient->GetProjectionMatrix();
    }

    // 일단 지금은 staticMesh돌면서 업데이트 해줄 필요가 없어서 여기 넣는데, Prepare에 넣을지 아니면 여기 그대로 둘지는 좀 더 생각해봐야함.
    // 매프레임 한번씩만 해줘도 충분하고 라이트 갯수가 변경될때만 해줘도 충분할듯하다
    // 지금 딸깍이에서 structuredBuffer도 처리해줘서 그 타이밍보고 나중에 다시 PSSetShaderResources를 해줘야함
   // UpdateComputeResource();
    
    UpdateCameraConstant(InViewportClient);
    
    for (UStaticMeshComponent* StaticMeshComp : StaticMeshComponents)
    {
        const FMatrix Model = StaticMeshComp->GetWorldMatrix();
        UpdateMatrixConstants(StaticMeshComp, View, Proj);
        // uint32 isSelected = 0;
        // if (GetWorld()->GetSelectedActors().Contains(staticMeshComp->GetOwner()))
        // {
        //     isSelected = 1;
        // }
        // UpdateSkySphereTextureConstants(Cast<USkySphereComponent>(StaticMeshComp));
        // UpdateContstantBufferActor(UUIDColor , isSelected);

        UpdateLightConstants();

        UpdateFlagConstant();
        
        //UpdateComputeConstants(InViewportClient);
        
        if (curEditorViewportClient->GetShowFlag() & static_cast<uint64>(EEngineShowFlags::Type::SF_AABB))
        {
            TSet<AActor*> Actors = InViewportClient->GetWorld()->GetSelectedActors();
            if (!Actors.IsEmpty() && *Actors.begin() == StaticMeshComp->GetOwner())
            {
                UPrimitiveBatch::GetInstance().AddAABB(
                    StaticMeshComp->GetBoundingBox(),
                    StaticMeshComp->GetWorldLocation(),
                    Model
                );
            }
        }

        if (!StaticMeshComp->GetStaticMesh()) continue;
        
        const OBJ::FStaticMeshRenderData* renderData = StaticMeshComp->GetStaticMesh()->GetRenderData();
        if (renderData == nullptr) continue;

        // VIBuffer Bind
        const std::shared_ptr<FVBIBTopologyMapping> VBIBTopMappingInfo = Renderer.GetVBIBTopologyMapping(StaticMeshComp->GetVBIBTopologyMappingName());
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
            
            UpdateMaterialConstants(StaticMeshComp->GetMaterial(materialIndex)->GetMaterialInfo());

            // index draw
            const uint64 startIndex = renderData->MaterialSubsets[subMeshIndex].IndexStart;
            const uint64 indexCount = renderData->MaterialSubsets[subMeshIndex].IndexCount;
            Graphics.DeviceContext->DrawIndexed(indexCount, startIndex, 0);
        }
    }

    ID3D11ShaderResourceView* nullSRVs[8] = { nullptr };
    ID3D11ShaderResourceView* nullSRV[4] = { nullptr };
    Graphics.DeviceContext->PSSetShaderResources(2, 1, &nullSRVs[0]); //쓰고 해제 나중에 이쁘게 뺴기
    Graphics.DeviceContext->PSSetShaderResources(3, 8, nullSRVs);
    Graphics.DeviceContext->PSSetShaderResources(11, 4, nullSRV);
    Graphics.DeviceContext->PSSetShaderResources(15, 8, nullSRVs);
    Graphics.DeviceContext->PSSetShaderResources(23, 8, nullSRVs);
}

//void FStaticMeshRenderPass::UpdateComputeConstants(const std::shared_ptr<FViewportClient> InViewportClient)
//{
//    FRenderResourceManager* renderResourceManager = GEngineLoop.Renderer.GetResourceManager();
//    // MVP Update
//    FComputeConstants ComputeConstants;
//    
//    FEditorViewportClient* ViewPort = dynamic_cast<FEditorViewportClient*>(InViewportClient.get());
//    
//    int screenWidth = ViewPort->GetViewport()->GetScreenRect().Width;  // 화면 가로 픽셀 수
//    int screenHeight = ViewPort->GetViewport()->GetScreenRect().Height;  // 화면 세로 픽셀 수
//
//    // 타일 크기 (예: 16x16 픽셀)
//    const int TILE_SIZE_X = 16;
//    const int TILE_SIZE_Y = 16;
//
//    // 타일 개수 계산
//    int numTilesX = (screenWidth + TILE_SIZE_X - 1) / TILE_SIZE_X; // 1024/16=64
//    int numTilesY = (screenHeight + TILE_SIZE_Y - 1) / TILE_SIZE_Y; // 768/16=48
//    
//    FMatrix InvView = FMatrix::Identity;
//    FMatrix InvProj = FMatrix::Identity;
//    std::shared_ptr<FEditorViewportClient> curEditorViewportClient = std::dynamic_pointer_cast<FEditorViewportClient>(InViewportClient);
//    if (curEditorViewportClient != nullptr)
//    {
//        InvView = FMatrix::Inverse(curEditorViewportClient->GetViewMatrix());
//        InvProj = FMatrix::Inverse(curEditorViewportClient->GetProjectionMatrix());
//    }
//    
//    ComputeConstants.screenHeight = ViewPort->GetViewport()->GetScreenRect().Height;
//    ComputeConstants.screenWidth = ViewPort->GetViewport()->GetScreenRect().Width;
//    ComputeConstants.InverseProj = InvProj;
//    ComputeConstants.InverseView = InvView;
//    ComputeConstants.tileCountX = numTilesX;
//    ComputeConstants.tileCountY = numTilesY;
//
//    ID3D11Buffer* ComputeConstantBuffer = renderResourceManager->GetConstantBuffer(TEXT("FComputeConstants"));
//
//    renderResourceManager->UpdateConstantBuffer(ComputeConstantBuffer, &ComputeConstants);
//}

void FStaticMeshRenderPass::CreateDummyTexture()
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

void FStaticMeshRenderPass::ClearRenderObjects()
{
    StaticMeshComponents.Empty();
    LightComponents.Empty();
}

void FStaticMeshRenderPass::UpdateMatrixConstants(UStaticMeshComponent* InStaticMeshComponent, const FMatrix& InView, const FMatrix& InProjection)
{
    FRenderResourceManager* renderResourceManager = GEngineLoop.Renderer.GetResourceManager();
    // MVP Update
    const FMatrix Model = InStaticMeshComponent->GetWorldMatrix();
    const FMatrix NormalMatrix = FMatrix::Transpose(FMatrix::Inverse(Model));
    FMatrixConstants MatrixConstants;
    MatrixConstants.Model = Model;
    MatrixConstants.ViewProj = InView * InProjection;
    MatrixConstants.MInverseTranspose = NormalMatrix;
    if (InStaticMeshComponent->GetWorld()->GetSelectedActors().Contains(InStaticMeshComponent->GetOwner()))
    {
        MatrixConstants.isSelected = true;
    }
    else
    {
        MatrixConstants.isSelected = false;
    }
    renderResourceManager->UpdateConstantBuffer(TEXT("FMatrixConstants"), &MatrixConstants);
}

void FStaticMeshRenderPass::UpdateFlagConstant()
{
    FRenderResourceManager* renderResourceManager = GEngineLoop.Renderer.GetResourceManager();

    FFlagConstants FlagConstant;

    FlagConstant.IsLit = GEngineLoop.Renderer.bIsLit;
    FlagConstant.IsNormal = GEngineLoop.Renderer.bIsNormal;
    FlagConstant.IsVSM = GEngineLoop.Renderer.GetShadowFilterMode();

    renderResourceManager->UpdateConstantBuffer(TEXT("FFlagConstants"), &FlagConstant);
}

void FStaticMeshRenderPass::UpdateLightConstants()
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
            if (DummyWhiteTextureSRV == nullptr)
            {
                CreateDummyTexture();
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
    Graphics.DeviceContext->VSSetConstantBuffers(1, 1, &LightConstantBuffer);
    Graphics.DeviceContext->PSSetConstantBuffers(2, 1, &LightConstantBuffer);
}

// void FStaticMeshRenderPass::UpdateContstantBufferActor(const FVector4 UUID, int32 isSelected)
// {
//     FRenderResourceManager* renderResourceManager = GEngineLoop.Renderer.GetResourceManager();
//     
//     FConstatntBufferActor ConstatntBufferActor;
//
//     ConstatntBufferActor.UUID = UUID;
//     ConstatntBufferActor.IsSelectedActor = isSelected;
//     
//     renderResourceManager->UpdateConstantBuffer(TEXT("FConstatntBufferActor"), &ConstatntBufferActor);
// }

void FStaticMeshRenderPass::UpdateMaterialConstants(const FObjMaterialInfo& MaterialInfo)
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

void FStaticMeshRenderPass::UpdateCameraConstant(const std::shared_ptr<FViewportClient>& InViewportClient)
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

bool FStaticMeshRenderPass::IsLightInFrustum(ULightComponentBase* LightComponent, const FFrustum& CameraFrustum) const
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

bool FStaticMeshRenderPass::IsSpotLightInFrustum(USpotLightComponent* SpotLightComp, const FFrustum& CameraFrustum) const
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
