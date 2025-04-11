#include "FBaseRenderPass.h"

#include "EditorEngine.h"

extern UEditorEngine* GEngine;

void FBaseRenderPass::Prepare(std::shared_ptr<FViewportClient> InViewportClient)
{
    GEngine->renderer.PrepareShader(ShaderName);
}
