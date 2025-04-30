#pragma once
#include "FBaseRenderPass.h"

class FBlurRenderPass : public FBaseRenderPass
{
public:
    explicit FBlurRenderPass(const FName& InShaderName);

    virtual ~FBlurRenderPass() {}
    void AddRenderObjectsToRenderPass(UWorld* InWorld) override;
    void Prepare(std::shared_ptr<FViewportClient> InViewportClient) override;
    void Execute(std::shared_ptr<FViewportClient> InViewportClient) override;
    bool bRender;

    class ID3D11Buffer* BlurConstantBuffer = nullptr;

    void UpdateBlurConstant();
};
