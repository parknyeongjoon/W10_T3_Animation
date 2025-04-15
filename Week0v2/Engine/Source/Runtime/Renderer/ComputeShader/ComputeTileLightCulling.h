#pragma once
#include "Container/Array.h"
#include "Renderer/RenderPass/FBaseRenderPass.h"
#include "D3D11RHI/GraphicDevice.h"

class ULightComponentBase;
class USkySphereComponent;
struct FObjMaterialInfo;
struct FMatrix;
class UStaticMeshComponent;

class FComputeTileLightCulling
{
public:
    explicit FComputeTileLightCulling(const FName& InShaderName)
    {}

    virtual ~FComputeTileLightCulling() {}
    void AddRenderObjectsToRenderPass(UWorld* InWorld);
    void Dispatch(std::shared_ptr<FViewportClient> InViewportClient);
    void UpdateLightConstants();
    // void UpdateStructuredBuffer();

private:
    void UpdateComputeConstants(const std::shared_ptr<FViewportClient> InViewportClient);

    int PreviousLightCount = 0;
    TArray<ULightComponentBase*> LightComponents;
};
