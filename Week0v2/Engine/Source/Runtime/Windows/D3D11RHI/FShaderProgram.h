#pragma once
#include "Define.h"

class FShaderProgram
{
public:
    FShaderProgram(const FName InVSName, const FName InPSName, ID3D11InputLayout* InInputLayout)
        : VSName(InVSName), PSName(InPSName), InputLayout(InInputLayout)
    {}

    FShaderProgram() = default;

    // 셰이더 및 입력 레이아웃 바인딩 함수
    void Bind(ID3D11DeviceContext* context) const;

    void Release()
    {
        if (InputLayout)
        {
            InputLayout->Release();
            InputLayout = nullptr;
        }
    }
    
    FName GetVertexShaderName() const { return VSName; }
    FName GetPixelShaderName() const { return PSName; }
    ID3D11InputLayout*  GetInputLayout() const { return InputLayout; }
    
    void SetVertexShaderName(const FName& InName) { VSName = InName;}
    void SetPixelShaderName(const FName& InName) { PSName = InName;}
    void SetInputLayout(ID3D11InputLayout* InInputLayout) { InputLayout = InInputLayout;}
private:
    FName VSName;
    FName PSName;
    ID3D11InputLayout*  InputLayout;
};
