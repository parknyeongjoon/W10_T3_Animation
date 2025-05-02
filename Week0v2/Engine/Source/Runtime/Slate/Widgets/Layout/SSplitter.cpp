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

void SSplitter::OnResize(float width, float height)
{
}

bool SSplitter::OnPressed(FPoint coord)
{
    if (!IsHover(coord))
        return false;
    
    return bIsPressed = true;
}

bool SSplitter::OnReleased()
{
    bIsPressed = false;
    return false;
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
    if (SideLT)
    {
        SideLT->Initialize(FRect(0.0f, 0.0f, initRect.leftTopX, initRect.height));
    }
    if (SideRB)
    {
        SideRB->Initialize(FRect(initRect.leftTopX + initRect.width, 0.0f, initRect.leftTopX, initRect.height));
    }
}

void SSplitterH::OnResize(float width, float height)
{
    Rect.height = height;
    Rect.leftTopX *= width;
    if (SideLT)
    {
        SideLT->Rect.height = height;
    }
    if (SideRB)
    {
        SideRB->Rect.leftTopX *= width;
        SideRB->Rect.width *= width;
        SideLT->Rect.height = height;

    }
    UpdateChildRects();
}

void SSplitterH::LoadConfig(const TMap<FString, FString>& config)
{
    // 각 키에 대해 기본값을 지정 (예: 기본 위치 및 크기)
    Rect.leftTopX = GetValueFromConfig(config, "SplitterH.X", GEngineLoop.GraphicDevice.screenWidth *0.5f);
    Rect.leftTopY = GetValueFromConfig(config, "SplitterH.Y", 0.0f);

    Rect.width = GetValueFromConfig(config, "SplitterH.Width", 20.0f);
    Rect.height = GetValueFromConfig(config, "SplitterH.Height", 10.0f); // 수평 스플리터는 높이 고정
    
    Rect.leftTopX *= GEngineLoop.GraphicDevice.screenWidth /GetValueFromConfig(config, "SplitterV.Width", 1000.0f);
}

void SSplitterH::SaveConfig(TMap<FString, FString>& config) const
{
    config["SplitterH.X"] = std::to_string(Rect.leftTopX);
    config["SplitterH.Y"] = std::to_string(Rect.leftTopY);
    config["SplitterH.Width"] = std::to_string(Rect.width);
    config["SplitterH.Height"] = std::to_string(Rect.height);
}

void SSplitterV::Initialize(FRect initRect)
{
    __super::Initialize(initRect);
    if (SideLT)
    {
        SideLT->Initialize(FRect(0.0f, 0.0f, initRect.width, initRect.leftTopY));
    }
    if (SideRB)
    {
        SideRB->Initialize(FRect(0.0f, initRect.leftTopY + initRect.height, initRect.width, initRect.leftTopY));
    }
}

void SSplitterV::OnResize(float width, float height)
{
    Rect.width = width;
    Rect.leftTopY *= height;
    if (SideLT)
    {
        SideLT->Rect.width = width;
    }
    if (SideRB)
    {
        SideRB->Rect.leftTopY *= height;
        SideRB->Rect.height *= height;
        SideRB->Rect.width = width;
    }
    UpdateChildRects();
}

void SSplitterV::LoadConfig(const TMap<FString, FString>& config)
{
    Rect.leftTopX = GetValueFromConfig(config, "SplitterV.X", 0.0f);
    Rect.leftTopY = GetValueFromConfig(config, "SplitterV.Y", GEngineLoop.GraphicDevice.screenHeight * 0.5f);
    Rect.width = GetValueFromConfig(config, "SplitterV.Width", 10); // 수직 스플리터는 너비 고정
    Rect.height = GetValueFromConfig(config, "SplitterV.Height", 20);

    Rect.leftTopY *= GEngineLoop.GraphicDevice.screenHeight / GetValueFromConfig(config, "SplitterH.Height", 1000.0f);

}

void SSplitterV::SaveConfig(TMap<FString, FString>& config) const
{
    config["SplitterV.X"] = std::to_string(Rect.leftTopX);
    config["SplitterV.Y"] = std::to_string(Rect.leftTopY);
    config["SplitterV.Width"] = std::to_string(Rect.width);
    config["SplitterV.Height"] = std::to_string(Rect.height);
}
