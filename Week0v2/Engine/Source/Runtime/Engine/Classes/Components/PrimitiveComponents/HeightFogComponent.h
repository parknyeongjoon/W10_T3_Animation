#pragma once
#include "PrimitiveComponent.h"
#include "Runtime/CoreUObject/UObject/ObjectMacros.h"

struct FHeightFogComponentInfo : public FPrimitiveComponentInfo
{
    DECLARE_ACTORCOMPONENT_INFO(FHeightFogComponentInfo);

    FVector FogColor;
    float FogDensity;
    float FogStart;
    float FogEnd;
    float FogBaseHeight;
    float HeightFallOff;
    bool bIsHeightFog;
    float MaxOpacity; // MaxOpacity 추가
    float LightShaftDensity; // 추가: 광선 밀도 [4]

    FHeightFogComponentInfo()
        : FPrimitiveComponentInfo()
        , FogColor(FVector(0.7f, 0.2f, 0.2f))
        , FogDensity(0.7f)
        , FogStart(30.0f)
        , FogEnd(100.0f)
        , FogBaseHeight(5.0f)
        , HeightFallOff(0.02f)
        , bIsHeightFog(true)
        , MaxOpacity(1.0f)
        , LightShaftDensity(1.0f)
    {
        InfoType = TEXT("FHeightFogComponentInfo");
        ComponentType = TEXT("UHeightFogComponent");
    }

    virtual void Copy(FActorComponentInfo& Other) override
    {
        FPrimitiveComponentInfo::Copy(Other);
        FHeightFogComponentInfo& HeightFogInfo = static_cast<FHeightFogComponentInfo&>(Other);
        HeightFogInfo.FogColor = FogColor;
        HeightFogInfo.FogDensity = FogDensity;
        HeightFogInfo.FogStart = FogStart;
        HeightFogInfo.FogEnd = FogEnd;
        HeightFogInfo.FogBaseHeight = FogBaseHeight;
        HeightFogInfo.HeightFallOff = HeightFallOff;
        HeightFogInfo.bIsHeightFog = bIsHeightFog;
        HeightFogInfo.MaxOpacity = MaxOpacity;
        HeightFogInfo.LightShaftDensity = LightShaftDensity;
    }

    virtual void Serialize(FArchive& ar) const override
    {
        FPrimitiveComponentInfo::Serialize(ar);
        ar << FogColor << FogDensity << FogStart << FogEnd
            << FogBaseHeight << HeightFallOff << bIsHeightFog
            << MaxOpacity << LightShaftDensity;
    }

    virtual void Deserialize(FArchive& ar) override
    {
        FPrimitiveComponentInfo::Deserialize(ar);
        ar >> FogColor >> FogDensity >> FogStart >> FogEnd
            >> FogBaseHeight >> HeightFallOff >> bIsHeightFog
            >> MaxOpacity >> LightShaftDensity;
    }
};
class UHeightFogComponent : public UPrimitiveComponent
{
    DECLARE_CLASS(UHeightFogComponent, UPrimitiveComponent)
public:
    UHeightFogComponent();
    UHeightFogComponent(const UHeightFogComponent& Other);

    FVector GetFogColor() const { return FogColor; }
    float GetFogDensity() const { return FogDensity; }
    float GetFogStart() const { return FogStart; }
    float GetFogEnd() const { return FogEnd; }
    float GetFogZPosition() const { return GetComponentLocation().z; }
    float GetFogBaseHeight() const { return FogBaseHeight; }
    float GetHeightFallOff() const { return HeightFallOff; }
    bool IsHeightFog() const { return bIsHeightFog; }
    float GetMaxOpacity() const { return MaxOpacity; }
    float GetLightShaftDensity() const { return LightShaftDensity; }

    void SetFogColor(const FVector& InColor);
    void SetFogDensity(float InDensity);
    void SetFogStart(float InStart);
    void SetFogEnd(float InEnd);
    void SetFogBaseHeight(float InHeight);
    void SetHeightFallOff(float InFalloff);
    void SetHeightFog(bool bEnabled);
    void SetMaxOpacity(float InMaxOpacity);
    void SetLightShaftDensity(float InDensity);

public:
    virtual void LoadAndConstruct(const FActorComponentInfo& Info);
    virtual std::shared_ptr<FActorComponentInfo> GetActorComponentInfo() override;

private:
    FVector FogColor;
    float FogDensity;
    float FogStart;
    float FogEnd;
    float FogBaseHeight;
    float HeightFallOff;
    bool bIsHeightFog;
    float MaxOpacity; // MaxOpacity 추가
    float LightShaftDensity; // 추가: 광선 밀도 [4]
};


