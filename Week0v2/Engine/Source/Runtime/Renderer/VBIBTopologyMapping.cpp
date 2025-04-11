#include "VBIBTopologyMapping.h"

#include "EditorEngine.h"

extern UEditorEngine* GEngine;

void FVBIBTopologyMapping::Bind() const
{
    FGraphicsDevice GraphicDevice = GEngine->graphicDevice;
    
    FRenderResourceManager* RenderReourcesManager = GEngine->renderer.GetResourceManager();
    ID3D11Buffer* VB = RenderReourcesManager->GetVertexBuffer(VBName);
    ID3D11Buffer* IB = RenderReourcesManager->GetIndexBuffer(IBName);
    
    if(VB != nullptr)
        GraphicDevice.DeviceContext->IASetVertexBuffers(0, 1, &VB, &Stride, &Offset);
    else
        GraphicDevice.DeviceContext->IASetVertexBuffers(0, 1, nullptr, &Stride, &Offset);

    GraphicDevice.DeviceContext->IASetPrimitiveTopology(Topology);
    
    if (IB != nullptr)
        GraphicDevice.DeviceContext->IASetIndexBuffer(IB, DXGI_FORMAT_R32_UINT, 0);
    else
        GraphicDevice.DeviceContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);
}
