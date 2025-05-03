#pragma once
#include "FBaseRenderPass.h"

class FFinalRenderPass : public FBaseRenderPass
{
public:
    explicit FFinalRenderPass(const FName& InShaderName);

    virtual ~FFinalRenderPass() {}
    void AddRenderObjectsToRenderPass() override;
    void Prepare(FRenderer* Renderer, std::shared_ptr<FViewportClient> InViewportClient, const FString& InShaderName = FString("")) override;
    void Execute(std::shared_ptr<FViewportClient> InViewportClient) override;
    bool bRender;
};
