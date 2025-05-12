#pragma once
#include "SlateCore/Widgets/SWindow.h"
#include "Container/Map.h"
#include "sstream"
#include "Container/String.h"
#include "Math/Point.h"

class SSplitter : public SWindow
{    
public:
    virtual void Initialize(FRect InitRect) override;
    virtual bool OnPressed(FPoint Coord) override;
    virtual bool OnReleased() override;

    virtual void OnDragStart(const FPoint& MousePos) { /* 초기화 */ }
    virtual void OnDrag(const FPoint& Delta) = 0; // 가로/세로에 따라 구현 다름.

protected:
    // 스플리터가 포함된 영역에 따라 자식 창의 Rect를 재계산하는 함수
    virtual void UpdateChildRects() = 0;

    virtual float GetSplitterLTCenter() = 0;
    virtual void ClampSplitRatio() {}    

public:
    virtual bool IsSplitterHovered(const FPoint& InPoint) const;
    virtual bool IsSplitterPressed() const { return bIsSplitterPressed; }

public:
    SWindow* SideLT; // Left or Top
    SWindow* SideRB; // Right or Bottom

    float SplitRatio = 0.5f; // 값 범위: [0, 1]

    uint32 SplitterLimitLT = 100; // Pixel Value

    uint32 SplitterHalfThickness = 5; // Pixel Value
    
protected:
    bool bIsSplitterPressed = false;

public:
    virtual void LoadConfig(const TMap<FString, FString>& Config);
    virtual void SaveConfig(TMap<FString, FString>& Config) const;

    template <typename T>
    T GetValueFromConfig(const TMap<FString, FString>& Config, const FString& Key, T DefaultValue) {
        if (const FString* Value = Config.Find(Key))
        {
            std::istringstream iss(**Value);
            T NewValue;
            if (iss >> NewValue)
            {
                return NewValue;
            }
        }
        return DefaultValue;
    }
};

class SSplitterH : public SSplitter
{
public:
    virtual void Initialize(FRect InitRect) override;
    virtual void Resize(float InWidth, float InHeight) override;

    virtual void ClampSplitRatio() override;    
    virtual float GetSplitterLTCenter() override;
    
    void OnDrag(const FPoint& Delta) override;
    void UpdateChildRects() override;
    
    virtual void LoadConfig(const TMap<FString, FString>& Config) override;
    virtual void SaveConfig(TMap<FString, FString>& Config) const override;
};

class SSplitterV : public SSplitter
{
public:
    virtual void Initialize(FRect InRect) override;
    virtual void Resize(float InWidth, float InHeight) override;

    virtual void ClampSplitRatio() override;
    virtual float GetSplitterLTCenter() override;
    
    virtual void OnDrag(const FPoint& Delta) override;
    virtual void UpdateChildRects() override;

    virtual void LoadConfig(const TMap<FString, FString>& Config) override;
    virtual void SaveConfig(TMap<FString, FString>& Config) const override;
};
