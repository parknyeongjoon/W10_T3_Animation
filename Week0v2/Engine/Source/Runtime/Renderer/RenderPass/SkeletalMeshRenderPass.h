#pragma once
#include <d3d11.h>

#include "FBaseRenderPass.h"
#include "Container/Array.h"

class USkeletalMeshComponent;
struct FMatrix;
struct FMaterialInfo;
struct FFrustum;
class USkySphereComponent;
class USpotLightComponent;
class ULightComponentBase;
class UStaticMeshComponent;

class FSkeletalMeshRenderPass : FBaseRenderPass
{
public:
    explicit FSkeletalMeshRenderPass(const FName& InShaderName);
    virtual ~FSkeletalMeshRenderPass() {}

public:
    void AddRenderObjectsToRenderPass() override;
    void Prepare(FRenderer* Renderer, std::shared_ptr<FViewportClient> InViewportClient, const FString& InShaderName = FString("")) override;
    void UpdateComputeResource();
    void Execute(std::shared_ptr<FViewportClient> InViewportClient) override;
    //void UpdateComputeConstants(std::shared_ptr<FViewportClient> InViewportClient);
    void CreateDummyTexture();

    void ClearRenderObjects() override;
private:
    static void UpdateMatrixConstants(USkeletalMeshComponent* InSkeletalMeshComponent, const FMatrix& InView, const FMatrix& InProjection);
    void UpdateFlagConstant();
    void UpdateLightConstants();
    // void UpdateContstantBufferActor(const FVector4 UUID, int32 isSelected);
    static void UpdateMaterialConstants(const FMaterialInfo& MaterialInfo);
    void UpdateCameraConstant(const std::shared_ptr<FViewportClient>& InViewportClient);
    bool IsLightInFrustum(ULightComponentBase* LightComponent, const FFrustum& CameraFrustum) const;
    bool IsSpotLightInFrustum(USpotLightComponent* SpotLightComp, const FFrustum& CameraFrustum) const;

private:        
    TArray<ULightComponentBase*> LightComponents;
    TArray<USkeletalMeshComponent*> SkeletalMeshComponents;
    ID3D11ShaderResourceView* DummyWhiteTextureSRV = nullptr;
    ID3D11SamplerState* ShadowMapSampler = nullptr;
    ID3D11Buffer* LightConstantBuffer = nullptr;
};
