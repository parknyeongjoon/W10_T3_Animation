#pragma once



//void FRenderer::RenderDebugDepth(std::shared_ptr<FEditorViewportClient> ActiveViewport)
//{
    // // 현재 뷰포트의 뷰모드가 Depth 인지 확인
    // if (ActiveViewport->GetViewMode() != EViewModeIndex::VMI_Depth)
    // {
    //     return;
    // }
    //
    // Graphics->DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    // Graphics->DeviceContext->CopyResource(Graphics->DepthCopyTexture, Graphics->DepthStencilBuffer);
    //
    //
    // Graphics->DeviceContext->PSSetSamplers(0, 1, &DebugDepthSRVSampler);
    // Graphics->DeviceContext->PSSetShaderResources(0, 1, &Graphics->DepthCopySRV);
    //
    // Graphics->DeviceContext->VSSetShader(DebugDepthVertexShader, nullptr, 0);
    // Graphics->DeviceContext->PSSetShader(DebugDepthPixelShader, nullptr, 0);
    //
    // FViewportConstants ViewportConstants;
    // ViewportConstants.ViewportWidth = ActiveViewport->Viewport->GetViewport().Width / Graphics->screenWidth;
    // ViewportConstants.ViewportHeight = ActiveViewport->Viewport->GetViewport().Height/ Graphics->screenHeight;
    // ViewportConstants.ViewportOffsetX = ActiveViewport->Viewport->GetViewport().TopLeftX/ Graphics->screenWidth;
    // ViewportConstants.ViewportOffsetY = ActiveViewport->Viewport->GetViewport().TopLeftY/ Graphics->screenHeight;
    //
    // D3D11_MAPPED_SUBRESOURCE mappedResource;
    // Graphics->DeviceContext->Map(ViewportConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    // memcpy(mappedResource.pData, &ViewportConstants, sizeof(FViewportConstants));
    // Graphics->DeviceContext->Unmap(ViewportConstantBuffer, 0);
    //
    // // 렌더링 시 샘플러 설정
    // Graphics->DeviceContext->VSSetConstantBuffers(0, 1, &CameraConstantBuffer);
    // Graphics->DeviceContext->PSSetConstantBuffers(0, 1, &CameraConstantBuffer);
    // Graphics->DeviceContext->PSSetConstantBuffers(1, 1, &ViewportConstantBuffer);
    //
    // Graphics->DeviceContext->Draw(4, 0);
//}