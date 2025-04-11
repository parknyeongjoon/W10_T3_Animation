#pragma once
#include "FBaseRenderPass.h"
#include "Container/Array.h"

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
private:
    static void UpdateMatrixConstants(UStaticMeshComponent* InStaticMeshComponent, const FMatrix& InView, const FMatrix& InProjection);
    static void UpdateSkySphereTextureConstants(const USkySphereComponent* InSkySphereComponent);
    static void UpdateMaterialConstants(const FObjMaterialInfo& MaterialInfo);
    
    TArray<UStaticMeshComponent*> StaticMesheComponents;
};
