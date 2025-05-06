#include "SSplitter.h"
#include "LaunchEngineLoop.h"

void SSplitter::Initialize(FRect initRect)
{
    __super::Initialize(initRect);
    if (SideLT == nullptr)
    {
        SideLT = new SWindow();
    }
    if (SideRB == nullptr)
    {
        SideRB = new SWindow();
    }
}

bool SSplitter::OnPressed(FPoint coord)
{
    if (!IsHover(coord))
    {
        return false;
    }

    bIsSplitterPressed = IsSplitterHovered(coord);
    
    return bIsPressed = true;
}

bool SSplitter::OnReleased()
{
    bIsPressed = false;
    bIsSplitterPressed = false;
    
    return false;
}

bool SSplitter::IsSplitterHovered(const FPoint& InPoint) const
{
    if (SideLT && SideLT->IsHover(InPoint))
    {
        return false;
    }
    if (SideRB && SideRB->IsHover(InPoint))
    {
        return false;
    }
    return true;
}

void SSplitter::LoadConfig(const TMap<FString, FString>& config)
{
}

void SSplitter::SaveConfig(TMap<FString, FString>& config) const
{
}

void SSplitterH::Initialize(FRect initRect)
{
    __super::Initialize(initRect);
    UpdateChildRects();
}

void SSplitterH::Resize(float InWidth, float InHeight)
{
    __super::Resize(InWidth, InHeight);

    UpdateChildRects();
}

void SSplitterH::ClampSplitRatio()
{
    SplitRatio = FMath::Max(SplitRatio, static_cast<float>(SplitterLimitLT) / Rect.Width);
    SplitRatio = FMath::Min(SplitRatio, (Rect.Width - static_cast<float>(SplitterLimitLT)) / Rect.Width);
}

float SSplitterH::GetSplitterLTCenter()
{
    ClampSplitRatio();
    return Rect.Width * SplitRatio;
}

void SSplitterH::OnDrag(const FPoint& Delta)
{
    // 수평 스플리터의 경우, 좌우로 이동
    float CenterX = GetSplitterLTCenter();
    CenterX += Delta.X;

    // 픽셀 단위 이동을 위해 정수형으로 변환 후 계산
    SplitRatio = std::trunc(CenterX) / Rect.Width;
    
    UpdateChildRects();
}

void SSplitterH::UpdateChildRects()
{
    // 픽셀 단위로 계산하기 위해 정수형으로 변환
    const uint32 SplitterCenterX = static_cast<uint32>(GetSplitterLTCenter());
    
    if (SideLT)
    {
        SideLT->Initialize(FRect(
            0.0f,
            0.0f,
            static_cast<float>(SplitterCenterX - SplitterHalfThickness),
            std::trunc(Rect.Height)
        ));
    }
    if (SideRB)
    {
        const float Offset = static_cast<float>(SplitterCenterX + SplitterHalfThickness);
        
        SideRB->Initialize(FRect(
            Offset,
            0.0f,
            std::trunc(Rect.Width - Offset),
            std::trunc(Rect.Height)
        ));
    }
}

void SSplitterH::LoadConfig(const TMap<FString, FString>& config)
{
    // 각 키에 대해 기본값을 지정 (예: 기본 위치 및 크기)
    Rect.LeftTopX = GetValueFromConfig(config, "SplitterH.X", GEngineLoop.GraphicDevice.GetDefaultWindowData().ScreenWidth * 0.5f);
    Rect.LeftTopY = GetValueFromConfig(config, "SplitterH.Y", 0.0f);

    Rect.Width = GetValueFromConfig(config, "SplitterH.Width", 20.0f);
    Rect.Height = GetValueFromConfig(config, "SplitterH.Height", 10.0f); // 수평 스플리터는 높이 고정
    
    Rect.LeftTopX *= GEngineLoop.GraphicDevice.GetDefaultWindowData().ScreenWidth / GetValueFromConfig(config, "SplitterV.Width", 1000.0f);
}

void SSplitterH::SaveConfig(TMap<FString, FString>& config) const
{
    config["SplitterH.X"] = std::to_string(Rect.LeftTopX);
    config["SplitterH.Y"] = std::to_string(Rect.LeftTopY);
    config["SplitterH.Width"] = std::to_string(Rect.Width);
    config["SplitterH.Height"] = std::to_string(Rect.Height);
}

void SSplitterV::Initialize(FRect InRect)
{
    __super::Initialize(InRect);

    UpdateChildRects();
}

void SSplitterV::Resize(float InWidth, float InHeight)
{
    __super::Resize(InWidth, InHeight);

    UpdateChildRects();
}

void SSplitterV::ClampSplitRatio()
{
    SplitRatio = FMath::Max(SplitRatio, static_cast<float>(SplitterLimitLT) / Rect.Height);
    SplitRatio = FMath::Min(SplitRatio, (Rect.Height - static_cast<float>(SplitterLimitLT)) / Rect.Height);
}

float SSplitterV::GetSplitterLTCenter()
{
    ClampSplitRatio();
    return Rect.Height * SplitRatio;
}

void SSplitterV::OnDrag(const FPoint& Delta)
{
    float CenterY = GetSplitterLTCenter();
    CenterY += Delta.Y;

    // 픽셀 단위 이동을 위해 정수형으로 변환 후 계산
    SplitRatio = std::trunc(CenterY) / Rect.Height;
    
    UpdateChildRects();
}

void SSplitterV::UpdateChildRects()
{
    // 픽셀 단위로 계산하기 위해 정수형으로 변환
    const uint32 SplitterCenterY = static_cast<uint32>(GetSplitterLTCenter());
    
    if (SideLT)
    {
        SideLT->Initialize(FRect(
            0.0f,
            0.0f,
            std::trunc(Rect.Width),
            static_cast<float>(SplitterCenterY - SplitterHalfThickness)
        ));
    }
    if (SideRB)
    {
        const float Offset = static_cast<float>(SplitterCenterY + SplitterHalfThickness);
        
        SideRB->Initialize(FRect(
            0.0f,
            Offset,
            std::trunc(Rect.Width),
            std::trunc(Rect.Height - Offset)
        ));
    }
}

void SSplitterV::LoadConfig(const TMap<FString, FString>& config)
{
    Rect.LeftTopX = GetValueFromConfig(config, "SplitterV.X", 0.0f);
    Rect.LeftTopY = GetValueFromConfig(config, "SplitterV.Y", GEngineLoop.GraphicDevice.GetDefaultWindowData().ScreenHeight * 0.5f);
    Rect.Width = GetValueFromConfig(config, "SplitterV.Width", 10); // 수직 스플리터는 너비 고정
    Rect.Height = GetValueFromConfig(config, "SplitterV.Height", 20);

    Rect.LeftTopY *= GEngineLoop.GraphicDevice.GetDefaultWindowData().ScreenHeight / GetValueFromConfig(config, "SplitterH.Height", 1000.0f);

}

void SSplitterV::SaveConfig(TMap<FString, FString>& config) const
{
    config["SplitterV.X"] = std::to_string(Rect.LeftTopX);
    config["SplitterV.Y"] = std::to_string(Rect.LeftTopY);
    config["SplitterV.Width"] = std::to_string(Rect.Width);
    config["SplitterV.Height"] = std::to_string(Rect.Height);
}
