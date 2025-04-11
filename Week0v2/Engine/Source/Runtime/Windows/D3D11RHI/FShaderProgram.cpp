#include "FShaderProgram.h"

#include "EditorEngine.h"
#include "Renderer/Renderer.h"

extern UEditorEngine* GEngine;

void FShaderProgram::Bind() const
{
    const FGraphicsDevice GraphicDevice = GEngine->graphicDevice;
    FRenderResourceManager* RenderResourceManager = GEngine->renderer.GetResourceManager();
    
    ID3D11VertexShader* VertexShader = RenderResourceManager->GetVertexShader(VSName);
    ID3D11PixelShader* PixelShader = RenderResourceManager->GetPixelShader(PSName);

    if (VertexShader)
    {
        GraphicDevice.DeviceContext->VSSetShader(VertexShader, nullptr, 0);
    }
    else
    {
        GraphicDevice.DeviceContext->VSSetShader(nullptr, nullptr, 0);
    }

    if (PixelShader)
    {
        GraphicDevice.DeviceContext->PSSetShader(PixelShader, nullptr, 0);
    }
    else
    {
        GraphicDevice.DeviceContext->PSSetShader(nullptr, nullptr, 0);
    }

    if (InputLayout)
    {
        GraphicDevice.DeviceContext->IASetInputLayout(InputLayout);
    }
    else
    {
        GraphicDevice.DeviceContext->IASetInputLayout(nullptr);
    }
}

void FShaderProgram::Release()
{
    if (InputLayout)
    {
        InputLayout->Release();
        InputLayout = nullptr;
    }
}
