#pragma once
#include "FBaseRenderPass.h"

class FEditorArrowRenderPass : public FBaseRenderPass
{
public:
    explicit FEditorArrowRenderPass(const FName InShaderName)
        : FBaseRenderPass(InShaderName) {}
    ~FEditorArrowRenderPass() override;
    
    void AddRenderObjectsToRenderPass() override;
    void Prepare(FRenderer* Renderer, std::shared_ptr<FViewportClient> InViewportClient, const FString& InShaderName = FString("")) override;
    void Execute(std::shared_ptr<FViewportClient> InViewportClient) override;
};
