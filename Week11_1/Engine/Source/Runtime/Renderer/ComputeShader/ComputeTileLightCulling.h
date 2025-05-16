#pragma once
#include <memory>

#include "Container/Array.h"
#include "Math/Vector.h"

class FViewportClient;
class UWorld;
class FName;
class ULightComponentBase;

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