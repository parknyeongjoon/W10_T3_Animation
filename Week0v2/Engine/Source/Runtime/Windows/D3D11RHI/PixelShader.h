#pragma once
#include "Shader.h"

class FPixelShader : public FShader
{
public:
    FPixelShader(const FName InShaderName, const FString& InFullPath, ID3D11PixelShader* InPS, ID3DBlob* InShaderBlob,
                 D3D_SHADER_MACRO* InShaderMacro, const std::filesystem::file_time_type InWriteTime);
    void Bind() override;
    void Release() override;

    ID3D11PixelShader* GetPixelShader() const { return PS; }
    void UpdateShader() override;

private:
    ID3D11PixelShader* PS;
};
