#include "FBaseRenderPass.h"

#include "LaunchEngineLoop.h"
#include "Renderer/Renderer.h"

void FBaseRenderPass::Prepare(std::shared_ptr<FViewportClient> InViewportClient)
{
    GEngineLoop.Renderer.PrepareShader(ShaderName);
}
