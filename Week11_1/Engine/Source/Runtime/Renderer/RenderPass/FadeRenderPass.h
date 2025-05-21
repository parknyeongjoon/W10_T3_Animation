#pragma once
#include "PlayerCameraManager.h"
#include "FBaseRenderPass.h"
#include "Container/Array.h"


class FFadeRenderPass : public FBaseRenderPass
{
public:
    explicit FFadeRenderPass(const FName& InShaderName);

    virtual ~FFadeRenderPass() {}
    void AddRenderObjectsToRenderPass(UWorld* World) override;
    void Prepare(std::shared_ptr<FViewportClient> InViewportClient) override;
    void Execute(std::shared_ptr<FViewportClient> InViewportClient) override;
    void ClearRenderObjects() override;
    bool bRender;

    class ID3D11Buffer* FadeConstantBuffer = nullptr;

    void UpdateFadeConstant();
    
    TArray<APlayerCameraManager*> PlayerCameraManagers;
    
private:
    float AccumulatedFadeAlpha = 0.0f;
};
