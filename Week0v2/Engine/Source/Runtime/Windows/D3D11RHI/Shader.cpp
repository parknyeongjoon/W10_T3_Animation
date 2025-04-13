#include "Shader.h"

#include "EditorEngine.h"
#include "GraphicDevice.h"

bool FShader::IsOutDated() const
{
    const std::filesystem::file_time_type CurrentLastWriteTime = std::filesystem::last_write_time(*FullPath);
    if (LastWriteTime < CurrentLastWriteTime)
    {
        return true;
    }

    return false;
}

void FShader::Release()
{
    ShaderBlob->Release();
    ShaderBlob = nullptr;
}

void FShader::UpdateShader()
{
    const FGraphicsDevice GraphicDevice = GEngine->graphicDevice;
    FRenderer Renderer = GEngine->renderer;
    FRenderResourceManager* RenderResourceManager = GEngine->renderer.GetResourceManager();
    
}
