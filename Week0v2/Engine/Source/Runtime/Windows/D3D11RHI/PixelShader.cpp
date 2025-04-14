#include "PixelShader.h"

#include "EditorEngine.h"
#include "GraphicDevice.h"

FPixelShader::FPixelShader(const FName InShaderName, const FString& InFullPath, ID3D11PixelShader* InPS, ID3DBlob* InShaderBlob,
                           D3D_SHADER_MACRO* InShaderMacro,
                           const std::filesystem::file_time_type InWriteTime)
{
    ShaderName = InShaderName;
    FullPath = InFullPath;
    PS = InPS;
    ShaderBlob = InShaderBlob;
    ShaderMacro = InShaderMacro;
    LastWriteTime = InWriteTime;
}

void FPixelShader::Bind()
{
}

void FPixelShader::Release()
{
    PS->Release();
    PS = nullptr;
    
    FShader::Release();
}

void FPixelShader::UpdateShader()
{
    const FGraphicsDevice GraphicDevice = GEngine->graphicDevice;
    FRenderer Renderer = GEngine->renderer;
    FRenderResourceManager* RenderResourceManager = GEngine->renderer.GetResourceManager();

    RenderResourceManager->UpdatePixelShader(ShaderName.ToString(), ShaderMacro);
}
