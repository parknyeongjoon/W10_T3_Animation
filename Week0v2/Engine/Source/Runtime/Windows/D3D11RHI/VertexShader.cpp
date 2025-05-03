#include "VertexShader.h"

#include "EditorEngine.h"
#include "GraphicDevice.h"
#include "LaunchEngineLoop.h"
#include "Renderer/Renderer.h"

class FRenderResourceManager;

FVertexShader::FVertexShader(const FName InShaderName, const FString& InFullPath, ID3D11VertexShader* InVs, ID3DBlob* InShaderBlob,
                             D3D_SHADER_MACRO* InShaderMacro, const std::filesystem::file_time_type InWriteTime)
{
    ShaderName = InShaderName;
    FullPath = InFullPath;
    VS = InVs;
    ShaderBlob = InShaderBlob;
    if (InShaderMacro != nullptr)
        ShaderMacro = InShaderMacro;
    else
        ShaderMacro = nullptr;
    
    LastWriteTime = InWriteTime;
}

void FVertexShader::Bind()
{
    const FGraphicsDevice GraphicDevice = GEngineLoop.GraphicDevice;
    FRenderResourceManager* RenderResourceManager = GEngineLoop.Renderer.GetResourceManager();
    
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
    FRenderResourceManager* RenderResourceManager = GEngineLoop.Renderer.GetResourceManager();

    if (ShaderMacro != nullptr)
        RenderResourceManager->UpdateVertexShader(ShaderName.ToString(), FullPath, ShaderMacro);
    else
        RenderResourceManager->UpdateVertexShader(ShaderName.ToString(), FullPath, nullptr);
    
    const std::filesystem::file_time_type CurrentLastWriteTime = std::filesystem::last_write_time(*FullPath);
    LastWriteTime = CurrentLastWriteTime;
}
