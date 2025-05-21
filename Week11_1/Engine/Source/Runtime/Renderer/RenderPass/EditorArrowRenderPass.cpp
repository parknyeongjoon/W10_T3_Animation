#include "EditorArrowRenderPass.h"

FEditorArrowRenderPass::~FEditorArrowRenderPass()
{
}

void FEditorArrowRenderPass::AddRenderObjectsToRenderPass(UWorld* World)
{
}

void FEditorArrowRenderPass::Prepare(const std::shared_ptr<FViewportClient> InViewportClient)
{
    FBaseRenderPass::Prepare(InViewportClient);
}

void FEditorArrowRenderPass::Execute(const std::shared_ptr<FViewportClient> InViewportClient)
{
}
