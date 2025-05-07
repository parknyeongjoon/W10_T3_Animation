#include "ShadowRenderPass.h"
#include "Components/LightComponents/PointLightComponent.h"
#include "Components/LightComponents/SpotLightComponent.h"
#include "Renderer/Renderer.h"
#include "Math/JungleMath.h"
#include "Engine/Engine.h"
#include "LaunchEngineLoop.h"
#include "UnrealEd/EditorViewportClient.h"
#include "BaseGizmos/GizmoBaseComponent.h"
#include "Components/LightComponents/DirectionalLightComponent.h"
#include "Renderer/VBIBTopologyMapping.h"
#include "GameFramework/Actor.h"
#include "Components/Mesh/StaticMesh.h"
#include "Engine/FEditorStateManager.h"
#include "UObject/UObjectIterator.h"


FShadowRenderPass::FShadowRenderPass(const FName& InShaderName)
    :FBaseRenderPass(InShaderName)
{
    FGraphicsDevice& Graphics = GEngineLoop.GraphicDevice;
    D3D11_BUFFER_DESC cbDesc = {};
    cbDesc.ByteWidth = sizeof(FLightCameraConstant);
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbDesc.MiscFlags = 0;
    cbDesc.StructureByteStride = 0;
    HRESULT hr = Graphics.Device->CreateBuffer(&cbDesc, nullptr, &CameraConstantBuffer);
    if (FAILED(hr))
    {
        // 에러 처리
    }

    // atlas init
    // !TODO : 아틀라스를 1개 이상 사용할 수 있도록 수정
    SpotLightShadowMapAtlas = std::make_unique<FShadowMapAtlas>(Graphics.Device, EAtlasType::SpotLight2D);
    PointLightShadowMapAtlas = std::make_unique<FShadowMapAtlas>(Graphics.Device, EAtlasType::PointLightCube, 1024);
}

void FShadowRenderPass::AddRenderObjectsToRenderPass(UWorld* World)
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

        if (ULightComponentBase* LightComponentBase = Cast<ULightComponentBase>(SceneComponent))
        {
            Lights.Add(LightComponentBase);
        }
    }
}

void FShadowRenderPass::ClearRenderObjects()
{
    Lights.Empty();
    StaticMeshComponents.Empty();
}

void FShadowRenderPass::Prepare(std::shared_ptr<FViewportClient> InViewportClient)
{
    FBaseRenderPass::Prepare(InViewportClient);

    const FRenderer& Renderer = GEngineLoop.Renderer;
    const FGraphicsDevice& Graphics = GEngineLoop.GraphicDevice;

    Graphics.DeviceContext->VSSetConstantBuffers(0, 1, &CameraConstantBuffer);
    
    ID3D11ShaderResourceView* nullSRV = nullptr;
    Graphics.DeviceContext->PSSetShader(nullptr, nullptr, 0);
    Graphics.DeviceContext->PSSetShaderResources(0, 1, &nullSRV);
    Graphics.DeviceContext->OMSetDepthStencilState(Renderer.GetDepthStencilState(EDepthStencilState::LessEqual), 0);

    Graphics.DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 정정 연결 방식 설정
    Graphics.DeviceContext->RSSetState(Renderer.GetCurrentRasterizerState());
    
    ID3D11SamplerState* CompareSampler = Renderer.GetSamplerState(ESamplerType::ComparisonSampler);
    Graphics.DeviceContext->PSSetSamplers(static_cast<uint32>(ESamplerType::ComparisonSampler), 1, &CompareSampler);
}

void FShadowRenderPass::RenderPointLightShadowMap(UPointLightComponent* PointLight, FGraphicsDevice& Graphics)
{
    FShadowResource* Resource = PointLight->GetShadowResource();
    if (!Resource || Resource->GetAtlasSlotIndex() == -1)
        return;

    int slotIndex = Resource->GetAtlasSlotIndex();
    // 각 면마다 렌더링
    for (int faceIndex = 0; faceIndex < 6; ++faceIndex)
    {
        // 현재 면의 DSV 가져오기
        ID3D11DepthStencilView* CurrentFaceDSV = PointLightShadowMapAtlas->GetDSVCube(slotIndex, faceIndex);
        Graphics.DeviceContext->ClearDepthStencilView(CurrentFaceDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

        ID3D11ShaderResourceView* nullSRV = nullptr;
        Graphics.DeviceContext->PSSetShaderResources(0, 1, &nullSRV);
        Graphics.DeviceContext->OMSetRenderTargets(0, nullptr, CurrentFaceDSV);

        FMatrix CubeView = PointLight->GetViewMatrixForFace(faceIndex);
        FMatrix CubeProj= PointLight->GetProjectionMatrix();
        // 현재 면에 대한 뷰와 프로젝션 매트릭스로 렌더링
       RenderStaticMesh(CubeView, CubeProj);
    }
}

void FShadowRenderPass::Execute(std::shared_ptr<FViewportClient> InViewportClient)
{
    Prepare(InViewportClient);
    
    FGraphicsDevice& Graphics = GEngineLoop.GraphicDevice;
    FRenderer& Renderer = GEngineLoop.Renderer;

    FMatrix View = FMatrix::Identity;
    FMatrix Proj = FMatrix::Identity;

    std::shared_ptr<FEditorViewportClient> curEditorViewportClient = std::dynamic_pointer_cast<FEditorViewportClient>(InViewportClient);
    FMatrix CameraView = curEditorViewportClient->GetViewMatrix();
    FMatrix CameraProjection = curEditorViewportClient->GetProjectionMatrix();
    FFrustum CameraFrustum = FFrustum::ExtractFrustum(CameraView * CameraProjection);

    // 1. 가시성 있는 라이트를 모은다(Point, Spot), Directional은 알아서 렌더
    // 2. 모은 라이트로 아틀라스 구성
    // 3. 아틀라스 렌더

    // 가시성 있는 광원들을 타입별로 모은다
    TArray<UPointLightComponent*> VisiblePointLights;
    TArray<USpotLightComponent*> VisibleSpotLights;

    // 아틀라스 클리어하고간다 -> 하면안된다
    //SpotLightShadowMapAtlas->Clear2DSlots();
    //PointLightShadowMapAtlas->ClearCubeSlots();

    // DSV 클리어

    for (ULightComponentBase* Light : Lights)
    {
        if (!IsLightInFrustum(Light, CameraFrustum))
        {
            // 라이트 가시성이 없으면, 아틀라스 바인딩된 친구인지 확인하여 해제
            Light->GetShadowResource()->UnbindFromAtlas();
            continue;
        }

        if (UPointLightComponent* PointLight = Cast<UPointLightComponent>(Light))
        {
            FShadowResource* ShadowResource = PointLight->GetShadowResource();
            if (ShadowResource->IsInAtlas() == false)
            {
                int SlotIdx = PointLightShadowMapAtlas->AllocateCubeSlot();
                if (SlotIdx != -1)
                {
                    ShadowResource->BindToAtlas(PointLightShadowMapAtlas.get(), SlotIdx);
                    VisiblePointLights.Add(PointLight);
                }
                else
                {
                    // 아틀라스에 공간이 없다
                    UE_LOG(LogLevel::Warning, TEXT("Point Light Shadow Map Atlas is full!"));
                }
            }
            else
            {
                // 이미 아틀라스에 바인딩된 경우
                VisiblePointLights.Add(PointLight);
            }
        }
        else if (USpotLightComponent* SpotLight = Cast<USpotLightComponent>(Light))
        {
            FShadowResource* ShadowResource = SpotLight->GetShadowResource();
            if (ShadowResource->IsInAtlas() == false)
            {
                int SlotIdx = SpotLightShadowMapAtlas->Allocate2DSlot(1024);
                if (SlotIdx != -1)
                {
                    ShadowResource->BindToAtlas(SpotLightShadowMapAtlas.get(), SlotIdx);
                    VisibleSpotLights.Add(SpotLight);
                }
                else
                {
                    // 아틀라스에 공간이 없다
                    UE_LOG(LogLevel::Warning, TEXT("Point Light Shadow Map Atlas is full!"));
                }
            }
            else
            {
                // 이미 아틀라스에 바인딩된 경우
                VisibleSpotLights.Add(SpotLight);
            }
            
            //SetShaderResource(SpotLight->GetShadowResource());
        }
        else if (UDirectionalLightComponent* DirectionalLight = Cast<UDirectionalLightComponent>(Light))
        {
            // Directional Light는 아틀라스 사용 안함
            for (int i=0;i<CASCADE_COUNT;i++)
            {
                //TODO : Cascade 영역 따라서 해상도 바꿔가면서 Shadow 맵 그리기
                SetShaderResource(&DirectionalLight->GetShadowResource()[i]);
                View = DirectionalLight->GetCascadeViewMatrix(i);
                Proj = DirectionalLight->GetCascadeProjectionMatrix(i);
                RenderStaticMesh(View, Proj);
                //VSM
                //if (GEngine->renderer.GetShadowFilterMode() == EShadowFilterMode::VSM)
                //{
                //    FLOAT ClearColor[4] = { 0.25f, 0.25f, 0.25f, 1.0f };
                //    ID3D11RenderTargetView* RTV = DirectionalLight->GetShadowResource()[i].GetVSMRTV();
                //    Graphics.DeviceContext->ClearRenderTargetView(RTV, ClearColor);
                //    Graphics.DeviceContext->OMSetRenderTargets(1, &RTV, nullptr);
                //    GEngine->renderer.PrepareShader(TEXT("LightDepth"));
                //    ID3D11ShaderResourceView* SRV = DirectionalLight->GetShadowResource()[i].GetSRV();
                //    Graphics.DeviceContext->PSSetShaderResources(0, 1, &SRV);
                //    Graphics.DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
                //    ID3D11SamplerState* Sampler = Renderer.GetSamplerState(ESamplerType::Point);
                //    Graphics.DeviceContext->PSSetSamplers(0, 1, &Sampler);
                //    Graphics.DeviceContext->Draw(4, 0);
                //}
            }
        }
    }

    // 아틀라스에 바인딩된 라이트들에 대해 렌더링
    Graphics.DeviceContext->ClearDepthStencilView(SpotLightShadowMapAtlas->GetDSV2D(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    for (auto SpotLight : VisibleSpotLights)
    {
        // 아틀라스에 바인딩된 라이트들에 대해 렌더링
        Graphics.DeviceContext->OMSetRenderTargets(0, nullptr, SpotLightShadowMapAtlas->GetDSV2D());
        FShadowResource* ShadowResource = SpotLight->GetShadowResource();
        int slotIndex = ShadowResource->GetAtlasSlotIndex();

        // slot index로 뷰포트 세팅해서 할당
        D3D11_VIEWPORT Vp = {};
        
        const int SlotsPerRow = 4;
        const int SlotSize = 1024;

        int SlotX = slotIndex % SlotsPerRow;
        int SlotY = slotIndex / SlotsPerRow;

        Vp.TopLeftX = SlotX * SlotSize;
        Vp.TopLeftY = SlotY * SlotSize;
        Vp.Width = SlotSize;
        Vp.Height = SlotSize;
        Vp.MinDepth = 0.0f;
        Vp.MaxDepth = 1.0f;

        Graphics.DeviceContext->RSSetViewports(1, &Vp);

        View = SpotLight->GetViewMatrix();
        Proj = SpotLight->GetProjectionMatrix();
        RenderStaticMesh(View, Proj);
        //VSM
        if (GEngineLoop.Renderer.GetShadowFilterMode() == EShadowFilterMode::VSM)
        {
            if (!SpotLightShadowMapAtlas->GetVSMTexture2D())
                SpotLightShadowMapAtlas->CreateVSMResource(Graphics.Device, EAtlasType::SpotLight2D);
            D3D11_VIEWPORT VSMVp = {};
            VSMVp.TopLeftX = 0.0f;
            VSMVp.TopLeftY = 0.0f;
            VSMVp.Width = SHADOW_ATLAS_SIZE;
            VSMVp.Height = SHADOW_ATLAS_SIZE;
            VSMVp.MinDepth = 0.0f;
            VSMVp.MaxDepth = 1.0f;
            Graphics.DeviceContext->RSSetViewports(1, &VSMVp);
            FLOAT ClearColor[4] = { 0.25f, 0.25f, 0.25f, 1.0f };
            ID3D11RenderTargetView* RTV = SpotLightShadowMapAtlas->GetVSMRTV2D();
            Graphics.DeviceContext->ClearRenderTargetView(RTV, ClearColor);
            Graphics.DeviceContext->OMSetRenderTargets(1, &RTV, nullptr);
            GEngineLoop.Renderer.PrepareShader(TEXT("LightDepth"));
            ID3D11ShaderResourceView* SRV = SpotLightShadowMapAtlas->GetSRV2D();
            Graphics.DeviceContext->PSSetShaderResources(0, 1, &SRV);
            Graphics.DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
            ID3D11SamplerState* Sampler = Renderer.GetSamplerState(ESamplerType::Point);
            Graphics.DeviceContext->PSSetSamplers(0, 0, &Sampler);
            Graphics.DeviceContext->Draw(4, 0);
            Prepare(InViewportClient);
            Graphics.DeviceContext->RSSetViewports(1, &Vp);
        }
    }

    // 포인트 라이트 그림자 맵 렌더링

    for (auto PointLight : VisiblePointLights)
    {
        FShadowResource* ShadowResource = PointLight->GetShadowResource();
        D3D11_VIEWPORT Vp = {};

        Vp.TopLeftX = 0;
        Vp.TopLeftY = 0;
        Vp.Width = 1024;
        Vp.Height = 1024;
        Vp.MinDepth = 0.0f;
        Vp.MaxDepth = 1.0f;

        Graphics.DeviceContext->RSSetViewports(1, &Vp);

        RenderPointLightShadowMap(PointLight, Graphics);
    }

    Graphics.DeviceContext->RSSetViewports(1, &curEditorViewportClient->GetD3DViewport());
    const auto CurRTV = Graphics.GetCurrentRenderTargetView();
    Graphics.DeviceContext->OMSetRenderTargets(1, &CurRTV, Graphics.GetCurrentWindowData()->DepthStencilView);
    
    // 아틀라스 텍스쳐 바인딩
    ID3D11ShaderResourceView* SpotLightAtlasSRV = (GEngineLoop.Renderer.GetShadowFilterMode() == EShadowFilterMode::VSM) ?
        SpotLightShadowMapAtlas->GetVSMSRV2D() : SpotLightShadowMapAtlas->GetSRV2D();
    Graphics.DeviceContext->PSSetShaderResources(6, 1, &SpotLightAtlasSRV);
    ID3D11ShaderResourceView* PointLightAtlasSRV = PointLightShadowMapAtlas->GetSRVCube();
    Graphics.DeviceContext->PSSetShaderResources(7, 1, &PointLightAtlasSRV);
}

void FShadowRenderPass::SetShaderResource(FShadowResource* ShadowResource)
{
    FGraphicsDevice& Graphics = GEngineLoop.GraphicDevice;
    
    if (ShadowResource == nullptr)
        return;
        
    D3D11_VIEWPORT vp = ShadowResource->GetViewport();
    Graphics.DeviceContext->RSSetViewports(1, &vp);
    Graphics.DeviceContext->ClearDepthStencilView(ShadowResource->GetDSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    Graphics.DeviceContext->OMSetRenderTargets(0, nullptr, ShadowResource->GetDSV()); // 렌더 타겟 설정
}

void FShadowRenderPass::UpdateCameraConstant(FMatrix Model, FMatrix View, FMatrix Proj) const
{
    FRenderResourceManager* renderResourceManager = GEngineLoop.Renderer.GetResourceManager();

    FLightCameraConstant CameraConstants;
    CameraConstants.Model = Model;
    CameraConstants.View = View;
    CameraConstants.Proj = Proj;

    renderResourceManager->UpdateConstantBuffer(CameraConstantBuffer, &CameraConstants);
}

FShadowMapAtlas* FShadowRenderPass::GetShadowMapAtlas(ELightType LightType) const
{
    if (LightType == ELightType::PointLight)
    {
        return PointLightShadowMapAtlas.get();
    }
    else if (LightType == ELightType::SpotLight)
    {
        return SpotLightShadowMapAtlas.get();
    }
    return nullptr;
}

bool FShadowRenderPass::IsLightInFrustum(ULightComponentBase* LightComponent, const FFrustum& CameraFrustum) const
{
    if (Cast<UDirectionalLightComponent>(LightComponent))
        return true;

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

bool FShadowRenderPass::IsSpotLightInFrustum(USpotLightComponent* SpotLightComp, const FFrustum& CameraFrustum) const
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

void FShadowRenderPass::RenderStaticMesh(FMatrix View, FMatrix Projection)
{
    FMatrix Model = FMatrix::Identity;
    FRenderer& Renderer = GEngineLoop.Renderer;
    FGraphicsDevice& Graphics = GEngineLoop.GraphicDevice;
    
    for (const auto& StaticMesh : StaticMeshComponents)
    {
        if (!StaticMesh->GetStaticMesh()) continue;
        const OBJ::FStaticMeshRenderData* renderData = StaticMesh->GetStaticMesh()->GetRenderData();
        if (renderData == nullptr) continue;
        Model = StaticMesh->GetWorldMatrix();
        UpdateCameraConstant(Model, View, Projection);
        // VIBuffer Bind
        const std::shared_ptr<FVBIBTopologyMapping> VBIBTopMappingInfo = Renderer.GetVBIBTopologyMapping(StaticMesh->GetVBIBTopologyMappingName());
        VBIBTopMappingInfo->Bind();

        // If There's No Material Subset
        if (renderData->MaterialSubsets.Num() == 0)
        {
            Graphics.DeviceContext->DrawIndexed(VBIBTopMappingInfo->GetNumIndices(), 0, 0);
        }

        // SubSet마다 Material Update 및 Draw
        for (int subMeshIndex = 0; subMeshIndex < renderData->MaterialSubsets.Num(); ++subMeshIndex)
        {
            // index draw
            const uint64 startIndex = renderData->MaterialSubsets[subMeshIndex].IndexStart;
            const uint64 indexCount = renderData->MaterialSubsets[subMeshIndex].IndexCount;
            Graphics.DeviceContext->DrawIndexed(indexCount, startIndex, 0);
        }
    }
}