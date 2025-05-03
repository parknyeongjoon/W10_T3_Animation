#include "EditorArrowRenderPass.h"

FEditorArrowRenderPass::~FEditorArrowRenderPass()
{
}

void FEditorArrowRenderPass::AddRenderObjectsToRenderPass()
{
}

void FEditorArrowRenderPass::Prepare(const std::shared_ptr<FViewportClient> InViewportClient)
{
    FBaseRenderPass::Prepare(InViewportClient);
}

void FEditorArrowRenderPass::Execute(std::shared_ptr<FViewportClient> InViewportClient)
{
}
