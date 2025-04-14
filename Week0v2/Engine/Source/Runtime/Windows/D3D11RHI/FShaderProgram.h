#pragma once
#include "UObject/NameTypes.h"

class ID3D11InputLayout;

class FShaderProgram
{
public:
    FShaderProgram(const FName InVSName, const FName InPSName, const FName InInputLayoutName)
        : VSName(InVSName), PSName(InPSName), InputLayoutName(InInputLayoutName)
    {}

    FShaderProgram() = default;

    // 셰이더 및 입력 레이아웃 바인딩 함수
    void Bind() const;

    void Release();
    
    FName GetVertexShaderName() const { return VSName; }
    FName GetPixelShaderName() const { return PSName; }
    FName GetInputLayoutName() const { return InputLayoutName; }
    
    void SetVertexShaderName(const FName& InName) { VSName = InName;}
    void SetPixelShaderName(const FName& InName) { PSName = InName;}
    void SetInputLayoutName(const FName& InName) { InputLayoutName = InName; }
private:
    FName VSName;
    FName PSName;
    FName InputLayoutName;
};
