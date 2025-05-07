#pragma once
#include "FBaseRenderPass.h"
#include "Container/Array.h"

class USkySphereComponent;
struct FObjMaterialInfo;
struct FMatrix;
class UStaticMeshComponent;
class UGizmoBaseComponent;

class FGizmoRenderPass : public FBaseRenderPass
{
public:
    explicit FGizmoRenderPass(const FName& InShaderName)
        : FBaseRenderPass(InShaderName)
    {}

    virtual ~FGizmoRenderPass() {}
    void AddRenderObjectsToRenderPass(UWorld* World) override;
    void Prepare(std::shared_ptr<FViewportClient> InViewportClient) override;
    void Execute(std::shared_ptr<FViewportClient> InViewportClient) override;
    void ClearRenderObjects() override;
private:
    static void UpdateMatrixConstants(UGizmoBaseComponent* InGizmoComponent, const FMatrix& InView, const FMatrix& InProjection);
    static void UpdateMaterialConstants(const FObjMaterialInfo& MaterialInfo);
private:
    TArray<UGizmoBaseComponent*> GizmoComponents;
};
