#pragma once
#include "FBaseRenderPass.h"
#include "Container/Array.h"

class USkySphereComponent;
struct FMaterialInfo;
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
    void AddRenderObjectsToRenderPass() override;
    void Prepare(FRenderer* Renderer, std::shared_ptr<FViewportClient> InViewportClient, const FString& InShaderName = FString("")) override;
    void Execute(std::shared_ptr<FViewportClient> InViewportClient) override;
    void ClearRenderObjects() override;
private:
    static void UpdateMatrixConstants(UGizmoBaseComponent* InGizmoComponent, const FMatrix& InView, const FMatrix& InProjection);
    static void UpdateMaterialConstants(const FMaterialInfo& MaterialInfo);
private:
    TArray<UGizmoBaseComponent*> GizmoComponents;
};
