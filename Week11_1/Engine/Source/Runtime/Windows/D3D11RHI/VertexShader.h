#pragma once
#include "Shader.h"

class FVertexShader : public FShader
{
public:
    FVertexShader(FName InShaderName, const FString& InFullPath, ID3D11VertexShader* InVs, ID3DBlob* InShaderBlob, D3D_SHADER_MACRO* InShaderMacro,
                  std::filesystem::file_time_type InWriteTime);
    void Bind() override;
    void Release() override;

    ID3D11VertexShader* GetVertexShader() const { return VS; }
    void UpdateShader() override;

private:
    ID3D11VertexShader* VS;
};
