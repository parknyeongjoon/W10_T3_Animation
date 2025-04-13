#include "VertexShader.h"

FVertexShader::FVertexShader(const FName InShaderName, const FString& InFullPath, ID3D11VertexShader* InVs, ID3DBlob* InShaderBlob,
    const std::filesystem::file_time_type InWriteTime)
{
    ShaderName = InShaderName;
    FullPath = InFullPath;
    VS = InVs;
    ShaderBlob = InShaderBlob;
    LastWriteTime = InWriteTime;
}

void FVertexShader::Release()
{
    VS->Release();
    VS = nullptr;
    
    FShader::Release();
}
