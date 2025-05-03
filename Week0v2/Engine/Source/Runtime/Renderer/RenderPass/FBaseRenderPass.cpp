#include "FBaseRenderPass.h"

#include "Renderer/Renderer.h"

void FBaseRenderPass::Prepare(FRenderer* Renderer, std::shared_ptr<FViewportClient> InViewportClient, const FString& InShaderName)
{
    if (InShaderName.IsEmpty())
    {
        Renderer->PrepareShader(ShaderName);
    }
    else
    {
        Renderer->PrepareShader(InShaderName);
    }
}
