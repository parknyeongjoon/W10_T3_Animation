#pragma once

#include "Container/String.h"
#include "CoreUObject/UObject/NameTypes.h"

class FBaseRenderPass
{
public:
    FBaseRenderPass(const FName& InShaderName) : ShaderName(InShaderName) {}
    virtual ~FBaseRenderPass() {}

    virtual void AddRenderObjectsToRenderPass() = 0;
    virtual void Prepare(class FRenderer* Renderer, std::shared_ptr<class FViewportClient> InViewportClient, const FString& InShaderName = FString(""));

    virtual void Execute(std::shared_ptr<FViewportClient> InViewportClient) = 0;
    virtual void ClearRenderObjects() {};

protected:
    // 렌더패스에서 사용할 리소스들
    FName ShaderName;
};
