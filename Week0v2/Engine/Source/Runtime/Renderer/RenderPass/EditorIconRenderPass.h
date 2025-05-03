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
    
    void AddRenderObjectsToRenderPass() override;
    void Prepare(FRenderer* Renderer, std::shared_ptr<FViewportClient> InViewportClient, const FString& InShaderName = FString("")) override;
    void Execute(std::shared_ptr<FViewportClient> InViewportClient) override;
    void ClearRenderObjects() override;

private:
    TArray<UBillboardComponent*> BillboardComponents;
};
