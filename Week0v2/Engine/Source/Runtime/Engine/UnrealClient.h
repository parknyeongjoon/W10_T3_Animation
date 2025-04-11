#pragma once
#include "Define.h" 

enum class EViewScreenLocation
{
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight
};

class FViewport
{
public:
    FViewport();
    FViewport(EViewScreenLocation _viewLocation) : viewLocation(_viewLocation) {}
    ~FViewport();
    void Initialize();
    void ResizeViewport(const DXGI_SWAP_CHAIN_DESC& swapchaindesc);
    void ResizeViewport(FRect Top, FRect Bottom, FRect Left, FRect Right);
    void ResizeViewport(FRect newRect);

private:
    D3D11_VIEWPORT viewport;            // 뷰포트 정보
    EViewScreenLocation viewLocation;   // 뷰포트 위치
public:
    D3D11_VIEWPORT& GetViewport() { return viewport; }
    void SetViewport(D3D11_VIEWPORT _viewport) { viewport = _viewport; }
};

