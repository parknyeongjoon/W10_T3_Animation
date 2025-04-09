#pragma once
#include "PrimitiveComponent.h"
#include "Math/Color.h"
#include "Runtime/CoreUObject/UObject/ObjectMacros.h"

struct FHeightFogComponentInfo : public FPrimitiveComponentInfo
{
    DECLARE_ACTORCOMPONENT_INFO(FHeightFogComponentInfo);

    bool bIsActive;
    bool bIsExponential;
    float FogDensity;
    float HeightFogStart;
    float HeightFogEnd;
    float FogMaxOpacity;
    float DistanceFogNear;
    float DistanceFogFar;
    FLinearColor FogInscatteringColor;
    FLinearColor DirectionalInscatteringColor;
    FVector DirectionalLightDirection;
    float DirectionalInscatteringExponent;
    float DirectionalInscatteringStartDistance;

    FHeightFogComponentInfo()
        : FPrimitiveComponentInfo()
        , bIsActive(false)
        , bIsExponential(false)
        , FogDensity(0.0f)
        , HeightFogStart(0.0f)
        , HeightFogEnd(0.0f)
        , FogMaxOpacity(1.0f)
        , DistanceFogNear(0.0f)
        , DistanceFogFar(0.0f)
        , FogInscatteringColor(FLinearColor::White)
        , DirectionalInscatteringColor(FLinearColor::White)
        , DirectionalLightDirection(FVector::ZeroVector)
        , DirectionalInscatteringExponent(1.0f)
        , DirectionalInscatteringStartDistance(0.0f)
    {
        InfoType = TEXT("FHeightFogComponentInfo");
        ComponentType = TEXT("UHeightFogComponent");
    }

    virtual void Copy(FActorComponentInfo& Other) override
    {
        FPrimitiveComponentInfo::Copy(Other);
        FHeightFogComponentInfo& HeightFogInfo = static_cast<FHeightFogComponentInfo&>(Other);
        HeightFogInfo.bIsActive = bIsActive;
        HeightFogInfo.bIsExponential = bIsExponential;
        HeightFogInfo.FogDensity = FogDensity;
        HeightFogInfo.HeightFogStart = HeightFogStart;
        HeightFogInfo.HeightFogEnd = HeightFogEnd;
        HeightFogInfo.FogMaxOpacity = FogMaxOpacity;
        HeightFogInfo.DistanceFogNear = DistanceFogNear;
        HeightFogInfo.DistanceFogFar = DistanceFogFar;
        HeightFogInfo.FogInscatteringColor = FogInscatteringColor;
        HeightFogInfo.DirectionalInscatteringColor = DirectionalInscatteringColor;
        HeightFogInfo.DirectionalLightDirection = DirectionalLightDirection;
        HeightFogInfo.DirectionalInscatteringExponent = DirectionalInscatteringExponent;
        HeightFogInfo.DirectionalInscatteringStartDistance = DirectionalInscatteringStartDistance;
    }

    virtual void Serialize(FArchive& ar) const override
    {
        FPrimitiveComponentInfo::Serialize(ar);
        ar << bIsActive << bIsExponential << FogDensity << HeightFogStart << HeightFogEnd
            << FogMaxOpacity << DistanceFogNear << DistanceFogFar
            << FogInscatteringColor << DirectionalInscatteringColor
            << DirectionalLightDirection << DirectionalInscatteringExponent
            << DirectionalInscatteringStartDistance;
    }

    virtual void Deserialize(FArchive& ar) override
    {
        FPrimitiveComponentInfo::Deserialize(ar);
        ar >> bIsActive >> bIsExponential >> FogDensity >> HeightFogStart >> HeightFogEnd
            >> FogMaxOpacity >> DistanceFogNear >> DistanceFogFar
            >> FogInscatteringColor >> DirectionalInscatteringColor
            >> DirectionalLightDirection >> DirectionalInscatteringExponent
            >> DirectionalInscatteringStartDistance;
    }
};
class UHeightFogComponent : public UPrimitiveComponent
{
    DECLARE_CLASS(UHeightFogComponent, UPrimitiveComponent)
public:
    UHeightFogComponent();
    bool bIsActive;
    bool bIsExponential;
    float FogDensity;
    float HeightFogStart;
    float HeightFogEnd;
    float FogMaxOpacity;
    float DistanceFogNear;
    float DistanceFogFar;

    FLinearColor FogInscatteringColor;
    FLinearColor DirectionalInscatteringColor;
    FVector DirectionalLightDirection;
    float DirectionalInscatteringExponent;
    float DirectionalInscatteringStartDistance;

public:
    virtual void LoadAndConstruct(const FActorComponentInfo& Info);
    virtual std::shared_ptr<FActorComponentInfo> GetActorComponentInfo() override;
};


