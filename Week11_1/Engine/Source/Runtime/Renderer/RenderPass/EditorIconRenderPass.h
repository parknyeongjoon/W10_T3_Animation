#pragma once
#include "FBaseRenderPass.h"
#include "Container/Array.h"

class UBillboardComponent;
class ULightComponentBase;

class FEditorIconRenderPass : public FBaseRenderPass
{
public:
    explicit FEditorIconRenderPass(const FName& InShaderName)
        : FBaseRenderPass(InShaderName) {}
    
    ~FEditorIconRenderPass() override;
    
    void AddRenderObjectsToRenderPass(UWorld* World) override;
    void Prepare(std::shared_ptr<FViewportClient> InViewportClient) override;
    void Execute(std::shared_ptr<FViewportClient> InViewportClient) override;
    void ClearRenderObjects() override;

private:
    TArray<UBillboardComponent*> BillboardComponents;
};
