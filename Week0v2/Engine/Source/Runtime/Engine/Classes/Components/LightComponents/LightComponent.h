#pragma once
#include "LightComponentBase.h"

struct FLightComponentInfo : public FLightComponentBaseInfo
{
    DECLARE_ACTORCOMPONENT_INFO(FLightComponentInfo);

    float ShadowResolutionScale;
    float ShadowBias;
    float ShadowSlopeBias;
    float ShadowSharpen;
    
    FLightComponentInfo()
        : FLightComponentBaseInfo()
        , ShadowResolutionScale(0.0f)
        , ShadowBias(0.0f)
        , ShadowSlopeBias(0.0f)
        , ShadowSharpen(0.0f)
    {
        InfoType = TEXT("FLightComponentInfo");
        ComponentType = TEXT("ULightComponent");
    }

    virtual void Copy(FActorComponentInfo& Other) override
    {
        FLightComponentBaseInfo::Copy(Other);
        FLightComponentInfo& DirectionalLightInfo = static_cast<FLightComponentInfo&>(Other);
        DirectionalLightInfo.ShadowResolutionScale = ShadowResolutionScale;
        DirectionalLightInfo.ShadowBias = ShadowBias;
        DirectionalLightInfo.ShadowSlopeBias = ShadowSlopeBias;
        DirectionalLightInfo.ShadowSharpen = ShadowSharpen;
    }
    virtual void Serialize(FArchive& ar) const override
    {
        FLightComponentBaseInfo::Serialize(ar);
        ar << ShadowResolutionScale << ShadowBias << ShadowSlopeBias << ShadowSharpen;
    }
    virtual void Deserialize(FArchive& ar) override
    {
        FLightComponentBaseInfo::Deserialize(ar);
        ar >> ShadowResolutionScale >> ShadowBias >> ShadowSlopeBias >> ShadowSharpen;
    }
};

class ULightComponent : public ULightComponentBase
{
    DECLARE_CLASS(ULightComponent, ULightComponentBase)
public:
    ULightComponent();
    ULightComponent(const ULightComponent& Other);
    ~ULightComponent() override;
    
    void UninitializeComponent() override;
    void OnComponentDestroyed() override;
    void DestroyComponent() override;

    float GetShadowResolutionScale() const { return ShadowResolutionScale; }
    float GetShadowBias() const { return ShadowBias; }
    float GetShadowSlopeBias() const { return ShadowSlopeBias; }
    float GetShadowSharpen() const { return ShadowSharpen; }

    FORCEINLINE void SetShadowResolutionScale(const float InShadowResolutionScale) { ShadowResolutionScale = InShadowResolutionScale; }
    FORCEINLINE void SetShadowBias(const float InShadowBias) { ShadowBias = InShadowBias; }
    FORCEINLINE void SetShadowSlopeBias(const float InShadowSlopeBias) { ShadowSlopeBias = InShadowSlopeBias; }
    FORCEINLINE void SetShadowSharpen(const float InShadowSharpen) { ShadowSharpen = InShadowSharpen; }

protected:
    void OnRegister() override;
    void OnUnregister() override;

    float ShadowResolutionScale = 0.0f;
    float ShadowBias = 0.0f;
    float ShadowSlopeBias = 0.0f;
    float ShadowSharpen = 0.0f;
public:
    void TickComponent(float DeltaTime) override;
    int CheckRayIntersection(FVector& rayOrigin, FVector& rayDirection, float& pfNearHitDistance) override;
    void InitializeComponent() override;
    void BeginPlay() override;
    void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    UObject* Duplicate() const override;
    void DuplicateSubObjects(const UObject* Source) override;
    void PostDuplicate() override;
    std::shared_ptr<FActorComponentInfo> GetActorComponentInfo() override;
    void LoadAndConstruct(const FActorComponentInfo& Info) override;
};
