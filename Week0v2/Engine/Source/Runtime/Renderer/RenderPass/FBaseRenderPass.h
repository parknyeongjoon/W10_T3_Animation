#pragma once
#include <memory>

#include "CoreUObject/UObject/NameTypes.h"

class UWorld;
class FViewportClient;

class FBaseRenderPass
{
public:
    FBaseRenderPass(const FName& InShaderName)
        :  ShaderName(InShaderName) {}
    virtual ~FBaseRenderPass() {}

    virtual void AddRenderObjectsToRenderPass(UWorld* World) = 0;
    virtual void Prepare(std::shared_ptr<FViewportClient> InViewportClient);
    virtual void Execute(std::shared_ptr<FViewportClient> InViewportClient) = 0;
    virtual void ClearRenderObjects() {};

protected:
    // 렌더패스에서 사용할 리소스들
    FName ShaderName;
};
