#include "FBaseRenderPass.h"

#include "LaunchEngineLoop.h"
#include "Viewport.h"
#include "D3D11RHI/CBStructDefine.h"
#include "Renderer/Renderer.h"
#include "SlateCore/Layout/SlateRect.h"
#include "UnrealEd/EditorViewportClient.h"

void FBaseRenderPass::Prepare(std::shared_ptr<FViewportClient> InViewportClient)
{
    GEngineLoop.Renderer.PrepareShader(ShaderName);
}

void FBaseRenderPass::UpdateScreenConstant(std::shared_ptr<FViewportClient> InViewportClient)
{
    const FGraphicsDevice& Graphics = GEngineLoop.GraphicDevice;
    FRenderResourceManager* renderResourceManager = GEngineLoop.Renderer.GetResourceManager();
    std::shared_ptr<FEditorViewportClient> curEditorViewportClient = std::dynamic_pointer_cast<FEditorViewportClient>(InViewportClient);

    FViewportInfo ScreenConstants;
    float Width = Graphics.GetCurrentWindowData()->ScreenWidth;
    float Height = Graphics.GetCurrentWindowData()->ScreenHeight;
    ScreenConstants.ViewportSize = FVector2D { curEditorViewportClient->GetViewport()->GetFSlateRect().Width / Width, curEditorViewportClient->GetViewport()->GetFSlateRect().Height / Height };
    ScreenConstants.ViewportOffset = FVector2D { curEditorViewportClient->GetViewport()->GetFSlateRect().LeftTopX / Width, curEditorViewportClient->GetViewport()->GetFSlateRect().LeftTopY / Height };

    renderResourceManager->UpdateConstantBuffer(TEXT("FViewportInfo"), &ScreenConstants);
}
