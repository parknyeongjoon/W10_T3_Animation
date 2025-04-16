#pragma once
#include "Container/Array.h"
#include "Renderer/RenderPass/FBaseRenderPass.h"
#include "D3D11RHI/GraphicDevice.h"
#include "Math/Vector.h"

class ULightComponentBase;
class USkySphereComponent;
struct FObjMaterialInfo;
struct FMatrix;
class UStaticMeshComponent;

class FComputeTileLightCulling
{
public:
    explicit FComputeTileLightCulling(const FName& InShaderName);

    virtual ~FComputeTileLightCulling() {}
    void AddRenderObjectsToRenderPass(UWorld* InWorld);
    void OnResize(int screenWidth, int screenHeight);
    void Dispatch(std::shared_ptr<FViewportClient> InViewportClient);
    void UpdateLightConstants();
    
private:
    void UpdateComputeConstants(const std::shared_ptr<FViewportClient> InViewportClient, int NumTileX, int NumTileY);

    FVector2D PreviousTileCount;
    TArray<ULightComponentBase*> LightComponents;
};