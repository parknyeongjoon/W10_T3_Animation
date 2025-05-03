#pragma once
#include "PlayerCameraManager.h"
#include "FBaseRenderPass.h"
#include "Container/Array.h"


class FFadeRenderPass : public FBaseRenderPass
{
public:
    explicit FFadeRenderPass(const FName& InShaderName);

    virtual ~FFadeRenderPass() {}
    void AddRenderObjectsToRenderPass() override;
    void Prepare(FRenderer* Renderer, std::shared_ptr<FViewportClient> InViewportClient, const FString& InShaderName = FString("")) override;
    void Execute(std::shared_ptr<FViewportClient> InViewportClient) override;
    void ClearRenderObjects() override;
    bool bRender;

    class ID3D11Buffer* FadeConstantBuffer = nullptr;

    void UpdateFadeConstant();

    TArray<APlayerCameraManager*> PlayerCameraManagers;
};
