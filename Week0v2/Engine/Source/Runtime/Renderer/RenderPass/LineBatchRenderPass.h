#pragma once
#include "FBaseRenderPass.h"
#include "Define.h"
class UShapeComponent;
class UCapsuleShapeComponent;
class FLineBatchRenderPass : public FBaseRenderPass
{
public:
    explicit FLineBatchRenderPass(const FName& InShaderName);

    void AddRenderObjectsToRenderPass() override;
    void Execute(std::shared_ptr<FViewportClient> InViewportClient) override;

    void Prepare(FRenderer* Renderer, std::shared_ptr<FViewportClient> InViewportClient, const FString& InShaderName = FString("")) override;
    void ClearRenderObjects() override;
private:
    static void UpdateBatchResources();
    TArray<UShapeComponent*> ShapeComponents;
    TArray<UCapsuleShapeComponent*> CapsuleShapeComponents;
    FName VBIBTopologyMappingName;
};
