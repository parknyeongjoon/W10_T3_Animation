#include "EditorArrowRenderPass.h"

FEditorArrowRenderPass::~FEditorArrowRenderPass()
{
}

void FEditorArrowRenderPass::AddRenderObjectsToRenderPass()
{
}

void FEditorArrowRenderPass::Prepare(FRenderer* Renderer, const std::shared_ptr<FViewportClient> InViewportClient, const FString& InShaderName)
{
    FBaseRenderPass::Prepare(Renderer, InViewportClient, InShaderName);
}

void FEditorArrowRenderPass::Execute(std::shared_ptr<FViewportClient> InViewportClient)
{
}
