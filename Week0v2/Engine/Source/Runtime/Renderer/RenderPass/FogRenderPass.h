#pragma once
#include "FBaseRenderPass.h"
#include "Windows/D3D11RHI/GraphicDevice.h"
#include <Components/HeightFogComponent.h>

class FFogRenderPass : public FBaseRenderPass
{
public:
    explicit FFogRenderPass(const FName& InShaderName);

    virtual ~FFogRenderPass() {}
    void AddRenderObjectsToRenderPass(UWorld* InWorld) override;
    void PrePrepare();
    void Prepare(std::shared_ptr<FViewportClient> InViewportClient) override;
    void Execute(std::shared_ptr<FViewportClient> InViewportClient) override;
    bool ShouldRender() { return bRender; }

private:
    void UpdateCameraConstant(const std::shared_ptr<FViewportClient> InViewportClient);
    void UpdateScreenConstant(std::shared_ptr<FViewportClient> InViewportClient);
    void UpdateFogConstant(const std::shared_ptr<FViewportClient> InViewportClient);

private:
    bool bRender = false;
    ID3D11Buffer* FogCameraConstantBuffer = nullptr;
    UHeightFogComponent* FogComp = nullptr;
};