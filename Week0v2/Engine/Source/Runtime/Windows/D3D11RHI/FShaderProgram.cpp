#include "FShaderProgram.h"

#include "LaunchEngineLoop.h"
#include "Renderer/Renderer.h"

void FShaderProgram::Bind() const
{
    const FGraphicsDevice GraphicDevice = GEngineLoop.GraphicDevice;
    FRenderResourceManager* RenderResourceManager = GEngineLoop.Renderer.GetResourceManager();
    
    ID3D11VertexShader* VertexShader = RenderResourceManager->GetVertexShader(VSName);
    ID3D11PixelShader* PixelShader = RenderResourceManager->GetPixelShader(PSName);
    ID3D11InputLayout* InputLayout = RenderResourceManager->GetInputLayout(InputLayoutName);

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
}
