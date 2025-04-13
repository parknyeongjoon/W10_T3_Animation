#pragma once
#include "Shader.h"

class FPixelShader : public FShader
{
public:
    FPixelShader(const FName InShaderName, const FString& InFullPath, ID3D11PixelShader* InPS, ID3DBlob* InShaderBlob,
    const std::filesystem::file_time_type InWriteTime);
    void Bind() override;
    void Release() override;

    ID3D11PixelShader* GetPixelShader() const { return PS; }
private:
    ID3D11PixelShader* PS;
};
