#include "VertexShader.h"

#include "EditorEngine.h"
#include "GraphicDevice.h"

FVertexShader::FVertexShader(const FName InShaderName, const FString& InFullPath, ID3D11VertexShader* InVs, ID3DBlob* InShaderBlob,
                             D3D_SHADER_MACRO* InShaderMacro, const std::filesystem::file_time_type InWriteTime)
{
    ShaderName = InShaderName;
    FullPath = InFullPath;
    VS = InVs;
    ShaderBlob = InShaderBlob;
    ShaderMacro = InShaderMacro;
    LastWriteTime = InWriteTime;
}

void FVertexShader::Bind()
{
    const FGraphicsDevice GraphicDevice = GEngine->graphicDevice;
    FRenderResourceManager* RenderResourceManager = GEngine->renderer.GetResourceManager();
    
    ID3D11VertexShader* VertexShader = RenderResourceManager->GetVertexShader(ShaderName);

    if (VertexShader)
    {
        GraphicDevice.DeviceContext->VSSetShader(VertexShader, nullptr, 0);
    }
    else
    {
        GraphicDevice.DeviceContext->VSSetShader(nullptr, nullptr, 0);
    }
}

void FVertexShader::Release()
{
    VS->Release();
    VS = nullptr;
    
    FShader::Release();
}

void FVertexShader::UpdateShader()
{
    const FGraphicsDevice GraphicDevice = GEngine->graphicDevice;
    FRenderer Renderer = GEngine->renderer;
    FRenderResourceManager* RenderResourceManager = GEngine->renderer.GetResourceManager();

    RenderResourceManager->UpdateVertexShader(ShaderName.ToString(), ShaderMacro);
    const std::filesystem::file_time_type CurrentLastWriteTime = std::filesystem::last_write_time(*FullPath);
    LastWriteTime = CurrentLastWriteTime;
}
