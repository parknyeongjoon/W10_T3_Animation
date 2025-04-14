#pragma once
#include "Define.h"
#include <filesystem>

class FShader
{
public:
    FName GetShaderName() const { return ShaderName; }
    void SetShaderName(const FName InShaderName) { ShaderName = InShaderName; }

    FString GetFullPath() { return FullPath; }
    void SetFullPath(const FString& InFullPath) { FullPath = InFullPath; }

    D3D_SHADER_MACRO* GetShaderMacro() const { return ShaderMacro; }
    void SetShaderMacro(D3D_SHADER_MACRO* InShaderMacro) { ShaderMacro = InShaderMacro; }
    
    ID3DBlob* GetShaderBlob() const { return ShaderBlob; }
    void SetShaderBlob(ID3DBlob* blob) { ShaderBlob = blob; }

    void SetRasterizerState(const ERasterizerState InRSState) { RasterizerState = InRSState; }
    void SetBlendState(const EBlendState InBlendState) { BlendState = InBlendState; }
    void SetDepthStencilState(const EDepthStencilState InDepthStencilState) { DepthStencilState = InDepthStencilState; }

    ERasterizerState GetRasterizerState() const { return RasterizerState; }
    EBlendState GetBlendState() const { return BlendState; }
    EDepthStencilState GetDepthStencilState() const { return DepthStencilState; }

    virtual void Bind() = 0;
    bool IsOutDated();
    virtual void UpdateShader() = 0;

    virtual void Release();
protected:
    FName ShaderName = TEXT("");
    FString FullPath = TEXT("");
    std::filesystem::file_time_type LastWriteTime;
    
    ID3DBlob* ShaderBlob = nullptr;
    D3D_SHADER_MACRO* ShaderMacro = nullptr;
    ERasterizerState RasterizerState = ERasterizerState::SolidBack;
    EBlendState BlendState = EBlendState::AlphaBlend;
    EDepthStencilState DepthStencilState = EDepthStencilState::LessEqual;
};
