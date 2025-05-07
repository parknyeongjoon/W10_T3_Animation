#pragma once
#include "SlateCore/Widgets/SWindow.h"
#include "Container/Map.h"
#include "sstream"
#include "Container/String.h"
#include "Math/Point.h"

class SSplitter : public SWindow
{    
public:
    virtual void Initialize(FRect initRect) override;
    virtual bool OnPressed(FPoint coord) override;
    virtual bool OnReleased() override;

    virtual void OnDragStart(const FPoint& mousePos) { /* 초기화 */ }
    virtual void OnDrag(const FPoint& delta) = 0; // 가로/세로에 따라 구현 다름.

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
    virtual void LoadConfig(const TMap<FString, FString>& config);
    virtual void SaveConfig(TMap<FString, FString>& config) const;

    template <typename T>
    T GetValueFromConfig(const TMap<FString, FString>& config, const FString& key, T defaultValue) {
        if (const FString* Value = config.Find(key))
        {
            std::istringstream iss(**Value);
            T value;
            if (iss >> value)
            {
                return value;
            }
        }
        return defaultValue;
    }
};

class SSplitterH : public SSplitter
{
public:
    virtual void Initialize(FRect initRect) override;
    virtual void Resize(float InWidth, float InHeight) override;

    virtual void ClampSplitRatio() override;    
    virtual float GetSplitterLTCenter() override;
    
    void OnDrag(const FPoint& delta) override;
    void UpdateChildRects() override;
    
    virtual void LoadConfig(const TMap<FString, FString>& config) override;
    virtual void SaveConfig(TMap<FString, FString>& config) const override;
};

class SSplitterV : public SSplitter
{
public:
    virtual void Initialize(FRect initRect) override;
    virtual void Resize(float InWidth, float InHeight) override;

    virtual void ClampSplitRatio() override;
    virtual float GetSplitterLTCenter() override;
    
    virtual void OnDrag(const FPoint& Delta) override;
    virtual void UpdateChildRects() override;

    virtual void LoadConfig(const TMap<FString, FString>& config) override;
    virtual void SaveConfig(TMap<FString, FString>& config) const override;

};
