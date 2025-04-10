#include "FShaderProgram.h"

#include "EditorEngine.h"
#include "Renderer/Renderer.h"

extern UEditorEngine* GEngine;

void FShaderProgram::Bind(ID3D11DeviceContext* context) const
{
    FRenderResourceManager RenderResourceManager = GEngine->renderer.GetResourceManager();
    
    ID3D11VertexShader* VertexShader = RenderResourceManager.GetVertexShader(VSName);
    ID3D11PixelShader* PixelShader = RenderResourceManager.GetPixelShader(PSName);

    if (VertexShader)
    {
        context->VSSetShader(VertexShader, nullptr, 0);
    }
    else
    {
        context->VSSetShader(nullptr, nullptr, 0);
    }

    if (PixelShader)
    {
        context->PSSetShader(PixelShader, nullptr, 0);
    }
    else
    {
        context->PSSetShader(nullptr, nullptr, 0);
    }

    if (InputLayout)
    {
        context->IASetInputLayout(InputLayout);
    }
    else
    {
        context->IASetInputLayout(nullptr);
    }
}