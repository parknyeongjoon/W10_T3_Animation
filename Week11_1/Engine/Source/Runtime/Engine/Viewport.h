#pragma once
#define _TCHAR_DEFINED
#include <d3d11.h>

#include "HAL/PlatformType.h"

class FRect;

enum class EViewScreenLocation : uint8
{
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight,
    Default,
    MAX,
};

class FViewport
{
public:
    FViewport();
    virtual ~FViewport() = default;

public:
    void ResizeViewport(FRect Top, FRect Bottom, FRect Left, FRect Right);
    void ResizeViewport(FRect NewRect);

public:
    D3D11_VIEWPORT& GetViewport() { return Viewport; }
    void SetViewport(const D3D11_VIEWPORT& InViewport) { Viewport = InViewport; }

    FRect GetFSlateRect() const;

    EViewScreenLocation ViewScreenLocation = EViewScreenLocation::Default;
    
private:
    D3D11_VIEWPORT Viewport = {};
};