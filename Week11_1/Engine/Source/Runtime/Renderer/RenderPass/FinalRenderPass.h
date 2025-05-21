#pragma once
#include "FBaseRenderPass.h"

class FFinalRenderPass : public FBaseRenderPass
{
public:
    explicit FFinalRenderPass(const FName& InShaderName);
    virtual ~FFinalRenderPass() {}
    
    void AddRenderObjectsToRenderPass(UWorld* World) override;
    void Prepare(std::shared_ptr<FViewportClient> InViewportClient) override;
    void Execute(std::shared_ptr<FViewportClient> InViewportClient) override;
    
    bool bRender;
};
