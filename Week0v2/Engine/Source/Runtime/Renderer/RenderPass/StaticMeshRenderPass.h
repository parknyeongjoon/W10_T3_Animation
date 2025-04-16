#pragma once
#include "FBaseRenderPass.h"
#include "Container/Array.h"
#include "Math/Vector4.h"

class ULightComponentBase;
class USkySphereComponent;
struct FObjMaterialInfo;
struct FMatrix;
class UStaticMeshComponent;

class FStaticMeshRenderPass : public FBaseRenderPass
{
public:
    explicit FStaticMeshRenderPass(const FName& InShaderName)
        : FBaseRenderPass(InShaderName)
    {}

    virtual ~FStaticMeshRenderPass() {}
    void AddRenderObjectsToRenderPass(UWorld* InWorld) override;
    void Prepare(std::shared_ptr<FViewportClient> InViewportClient) override;
    void Execute(std::shared_ptr<FViewportClient> InViewportClient) override;
    void ClearRenderObjects() override;
private:
    static void UpdateMatrixConstants(UStaticMeshComponent* InStaticMeshComponent, const FMatrix& InView, const FMatrix& InProjection);
    void UpdateFlagConstant();
    void UpdateLightConstants();
    void UpdateContstantBufferActor(const FVector4 UUID, int32 isSelected);
    static void UpdateSkySphereTextureConstants(const USkySphereComponent* InSkySphereComponent);
    static void UpdateMaterialConstants(const FObjMaterialInfo& MaterialInfo);
    void UpdateCameraConstant(const std::shared_ptr<FViewportClient>& InViewportClient);

private:
    TArray<ULightComponentBase*> LightComponents;
    TArray<UStaticMeshComponent*> StaticMesheComponents;
};
