#pragma once
#include "FBaseRenderPass.h"
#include "Windows/D3D11RHI/GraphicDevice.h"

class FDebugDepthRenderPass : public FBaseRenderPass
{
public:
    explicit FDebugDepthRenderPass(const FName& InShaderName)
        :FBaseRenderPass(InShaderName)
    {}

    virtual ~FDebugDepthRenderPass() {}
	void AddRenderObjectsToRenderPass(UWorld* World) override;
    void Prepare(std::shared_ptr<FViewportClient> InViewportClient) override;
    void Execute(std::shared_ptr<FViewportClient> InViewportClient) override;

private:
	void UpdateCameraConstant(const std::shared_ptr<FViewportClient> InViewportClient);
};
