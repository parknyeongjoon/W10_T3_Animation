#include "PixelShader.h"

FPixelShader::FPixelShader(const FName InShaderName, const FString& InFullPath, ID3D11PixelShader* InPS, ID3DBlob* InShaderBlob,
    const std::filesystem::file_time_type InWriteTime)
{
    ShaderName = InShaderName;
    FullPath = InFullPath;
    PS = InPS;
    ShaderBlob = InShaderBlob;
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
