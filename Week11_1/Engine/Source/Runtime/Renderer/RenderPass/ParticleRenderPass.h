#pragma once
#include "Define.h"
#include "FBaseRenderPass.h"
#include "Container/Array.h"

class UParticleSystemComponent;
class USpotLightComponent;
class ULightComponentBase;
class UStaticMeshComponent;

class FParticleRenderPass : public FBaseRenderPass
{
public:
    explicit FParticleRenderPass(const FName& InShaderName);

    virtual ~FParticleRenderPass() {}
    void AddRenderObjectsToRenderPass(UWorld* World) override;
    void Prepare(std::shared_ptr<FViewportClient> InViewportClient) override;
    void Execute(std::shared_ptr<FViewportClient> InViewportClient) override;
    //void UpdateComputeConstants(std::shared_ptr<FViewportClient> InViewportClient);
    void CreateDummyTexture();
    void ClearRenderObjects() override;

    // void UpdateComputeResource();
private:
    static void UpdateMatrixConstants(UParticleSystemComponent* InParticleSystemComponent, const FMatrix& InView, const FMatrix& InProjection);
    static void UpdateMaterialConstants(const FObjMaterialInfo& MaterialInfo);
    void UpdateFlagConstant();
    void UpdateLightConstants();
    void UpdateCameraConstant(const std::shared_ptr<FViewportClient>& InViewportClient);

private:
    bool IsLightInFrustum(ULightComponentBase* LightComponent, const FFrustum& CameraFrustum) const;
    bool IsSpotLightInFrustum(USpotLightComponent* SpotLightComp, const FFrustum& CameraFrustum) const;
private:        
    TArray<ULightComponentBase*> LightComponents;
    TArray<UParticleSystemComponent*> ParticleSystemComponents;
    
    ID3D11ShaderResourceView* DummyWhiteTextureSRV = nullptr;
    ID3D11SamplerState* ShadowMapSampler = nullptr;
    ID3D11Buffer* LightConstantBuffer = nullptr;
};
