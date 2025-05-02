#include "VBIBTopologyMapping.h"

#include "LaunchEngineLoop.h"
#include "Renderer.h"
#include "RenderResourceManager.h"
#include "D3D11RHI/GraphicDevice.h"
#include "Engine/Engine.h"

extern UEngine* GEngine;

void FVBIBTopologyMapping::Bind() const
{
    FGraphicsDevice GraphicDevice = GEngineLoop.GraphicDevice;
    
    FRenderResourceManager* RenderReourcesManager = GEngineLoop.Renderer.GetResourceManager();
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
