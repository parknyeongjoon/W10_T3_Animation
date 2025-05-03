#pragma once
#include "Core/HAL/PlatformType.h"

class ImGuiManager
{
public:
    void Initialize(HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext);
    void BeginFrame();
    void EndFrame();
    void Shutdown();
    
private:
    void PreferenceStyle();
};

