#include "PixelShader.h"

#include "GraphicDevice.h"
#include "LaunchEngineLoop.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderResourceManager.h"

FPixelShader::FPixelShader(const FName InShaderName, const FString& InFullPath, ID3D11PixelShader* InPS, ID3DBlob* InShaderBlob,
                           D3D_SHADER_MACRO* InShaderMacro,
                           const std::filesystem::file_time_type InWriteTime)
{
    ShaderName = InShaderName;
    FullPath = InFullPath;
    PS = InPS;
    ShaderBlob = InShaderBlob;
    if (InShaderMacro != nullptr)
        ShaderMacro = InShaderMacro;
    else
        ShaderMacro = nullptr;
    
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
    FRenderer Renderer = GEngineLoop.Renderer;
    FRenderResourceManager* RenderResourceManager = GEngineLoop.Renderer.GetResourceManager();

    if (ShaderMacro != nullptr)
        RenderResourceManager->UpdatePixelShader(ShaderName.ToString(), FullPath, ShaderMacro);
    else
        RenderResourceManager->UpdatePixelShader(ShaderName.ToString(), FullPath, nullptr);
    
    const std::filesystem::file_time_type CurrentLastWriteTime = std::filesystem::last_write_time(*FullPath);
    LastWriteTime = CurrentLastWriteTime;
}
