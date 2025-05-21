#pragma once
#include "Define.h"
#include "FBaseRenderPass.h"
#include "Container/Array.h"

class USkeletalMeshComponent;
class USpotLightComponent;
class ULightComponentBase;
class USkySphereComponent;
struct FObjMaterialInfo;
struct FMatrix;
class UStaticMeshComponent;


class FSkeletalMeshRenderPass : public FBaseRenderPass
{
public:
    explicit FSkeletalMeshRenderPass(const FName& InShaderName);

    virtual ~FSkeletalMeshRenderPass() {}
    void Prepare(std::shared_ptr<FViewportClient> InViewportClient) override;
    void Execute(std::shared_ptr<FViewportClient> InViewportClient) override;
    
    void AddRenderObjectsToRenderPass(UWorld* World) override;
    void ClearRenderObjects() override;
    
private:
    void UpdateMatrixConstants(USkeletalMeshComponent* InStaticMeshComponent, const FMatrix& InView, const FMatrix& InProjection);
    void UpdateFlagConstant();
    void UpdateLightConstants();
    void UpdateBoneConstant(USkeletalMeshComponent* SkeletalMeshComponent);
    void UpdateMaterialConstants(const FObjMaterialInfo& MaterialInfo);
    void UpdateCameraConstant(const std::shared_ptr<FViewportClient>& InViewportClient);

private:
    bool IsLightInFrustum(ULightComponentBase* LightComponent, const FFrustum& CameraFrustum) const;
    bool IsSpotLightInFrustum(USpotLightComponent* SpotLightComp, const FFrustum& CameraFrustum) const;
    
    TArray<ULightComponentBase*> LightComponents;
    TArray<USkeletalMeshComponent*> SkeletalMeshComponents;
    
    ID3D11Buffer* LightConstantBuffer = nullptr;
};
