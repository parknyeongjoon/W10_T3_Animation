#include "FogRenderPass.h"
#include <Define.h>
#include <EditorEngine.h>
#include <Components/QuadTexture.h>
#include <Components/HeightFogComponent.h>
#include <UObject/UObjectIterator.h>

FFogRenderPass::FFogRenderPass(const FName& InShaderName)
    :FBaseRenderPass(InShaderName)
{
    ID3D11Buffer* VB = UEditorEngine::renderer.GetResourceManager()->CreateImmutableVertexBuffer(quadTextureVertices, sizeof(quadTextureVertices));
    UEditorEngine::renderer.GetResourceManager()->AddOrSetVertexBuffer(TEXT("QuadVB"), VB);
    UEditorEngine::renderer.MappingVBTopology(TEXT("Quad"), TEXT("QuadVB"), sizeof(FVertexTexture), 4);

    ID3D11Buffer* IB = UEditorEngine::renderer.GetResourceManager()->CreateIndexBuffer(quadTextureInices, sizeof(quadTextureInices) / sizeof(uint32));
    UEditorEngine::renderer.GetResourceManager()->AddOrSetIndexBuffer(TEXT("QuadIB"), IB);
    UEditorEngine::renderer.MappingIB(TEXT("Quad"), TEXT("QuadIB"), sizeof(quadTextureInices) / sizeof(uint32));

    VBIBTopologyMappingName = TEXT("Quad");
}

void FFogRenderPass::AddRenderObjectsToRenderPass(UWorld* InWorld)
{
}

void FFogRenderPass::PrePrepare()
{
    FGraphicsDevice& Graphics = GEngine->graphicDevice;
    for (const auto iter : TObjectRange<UHeightFogComponent>())
    {
        if (iter->GetWorld() == GEngine->GetWorld())
        {
            Graphics.SwitchRTV();
            return;
        }
    }
}

void FFogRenderPass::Prepare(std::shared_ptr<FViewportClient> InViewportClient)
{
    FBaseRenderPass::Prepare(InViewportClient);
    FGraphicsDevice& Graphics = GEngine->graphicDevice;
}

void FFogRenderPass::Execute(std::shared_ptr<FViewportClient> InViewportClient)
{
    FGraphicsDevice& Graphics = GEngine->graphicDevice;

    Graphics.ReturnRTV();
}

void FFogRenderPass::UpdateCameraConstant(const std::shared_ptr<FViewportClient> InViewportClient)
{
}

void FFogRenderPass::UpdateScreenConstant(std::shared_ptr<FViewportClient> InViewportClient)
{
}
