#pragma once
#include "FBaseRenderPass.h"

class FEditorArrowRenderPass : public FBaseRenderPass
{
public:
    explicit FEditorArrowRenderPass(const FName InShaderName)
        : FBaseRenderPass(InShaderName) {}
    ~FEditorArrowRenderPass() override;
    
    void AddRenderObjectsToRenderPass() override;
    void Prepare(std::shared_ptr<FViewportClient> InViewportClient) override;
    void Execute(std::shared_ptr<FViewportClient> InViewportClient) override;
};
