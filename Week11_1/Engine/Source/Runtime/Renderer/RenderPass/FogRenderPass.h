#pragma once

#define _TCHAR_DEFINED
#include <d3d11.h>

#include "FBaseRenderPass.h"

class UHeightFogComponent;

class FFogRenderPass : public FBaseRenderPass
{
public:
    explicit FFogRenderPass(const FName& InShaderName);

    virtual ~FFogRenderPass() {}
    void AddRenderObjectsToRenderPass(UWorld* World) override;
    void ClearRenderObjects() override;
    void Prepare(std::shared_ptr<FViewportClient> InViewportClient) override;
    void Execute(std::shared_ptr<FViewportClient> InViewportClient) override;
    bool ShouldRender() const { return bRender; }

private:
    void UpdateCameraConstant(const std::shared_ptr<FViewportClient> InViewportClient) const;
    void UpdateFogConstant(const std::shared_ptr<FViewportClient> InViewportClient) const;

private:
    bool bRender = false;
    ID3D11Buffer* FogCameraConstantBuffer = nullptr;
    UHeightFogComponent* FogComp = nullptr;
};
