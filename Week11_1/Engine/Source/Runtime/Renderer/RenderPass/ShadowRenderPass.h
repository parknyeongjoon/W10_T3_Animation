#pragma once
#include "FBaseRenderPass.h"
#include "Windows/D3D11RHI/GraphicDevice.h"
#include "Components/PrimitiveComponents/MeshComponents/StaticMeshComponents/StaticMeshComponent.h"
#include <Components/LightComponents/SpotLightComponent.h>
#include "Light/ShadowMapAtlas.h"

class UPointLightComponent;
struct FLightCameraConstant
{
    FMatrix Model;
    FMatrix View;
    FMatrix Proj;
};

class FShadowRenderPass : public FBaseRenderPass
{
public:
    explicit FShadowRenderPass(const FName& InShaderName);

    virtual ~FShadowRenderPass() {}
    void AddRenderObjectsToRenderPass(UWorld* World) override;
    void ClearRenderObjects() override;

    void Prepare(std::shared_ptr<FViewportClient> InViewportClient) override;
    void RenderPointLightShadowMap(UPointLightComponent* PointLight, FGraphicsDevice& Graphics);
    void Execute(std::shared_ptr<FViewportClient> InViewportClient) override;
    void SetShaderResource(FShadowResource* ShadowResource);
    bool ShouldRender() { return bRender; }

private:
    bool IsLightInFrustum(ULightComponentBase* LightComponent, const FFrustum& CameraFrustum) const;
    bool IsSpotLightInFrustum(USpotLightComponent* SpotLightComp, const FFrustum& CameraFrustum) const;
    void RenderStaticMesh(FMatrix View, FMatrix Projection);

private:
    TArray<ULightComponentBase*> Lights;
    TArray<UStaticMeshComponent*> StaticMeshComponents;
    void UpdateCameraConstant(FMatrix Model, FMatrix View, FMatrix Proj) const;
    bool bRender = false;
    ID3D11Buffer* CameraConstantBuffer;

    // atlas
private:
    std::unique_ptr<FShadowMapAtlas> SpotLightShadowMapAtlas = nullptr;
    std::unique_ptr<FShadowMapAtlas> PointLightShadowMapAtlas = nullptr;

public:
    FShadowMapAtlas* GetShadowMapAtlas(ELightType LightType) const;
};