#pragma once
#include "FBaseRenderPass.h"

class FFadeRenderPass : public FBaseRenderPass
{
public:
    explicit FFadeRenderPass(const FName& InShaderName);

    virtual ~FFadeRenderPass() {}
    void AddRenderObjectsToRenderPass(UWorld* InWorld) override;
    void PrePrepare();
    void Prepare(std::shared_ptr<FViewportClient> InViewportClient) override;
    void Execute(std::shared_ptr<FViewportClient> InViewportClient) override;
    bool bRender;

    class ID3D11Buffer* FadeConstantBuffer = nullptr;

    void UpdateFadeConstant();
};
