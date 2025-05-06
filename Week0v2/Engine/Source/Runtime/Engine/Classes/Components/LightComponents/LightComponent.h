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
        ComponentClass = TEXT("ULightComponent");
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
    virtual ~ULightComponent() override;
    
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

    // 그림자 해상도 스케일 (1.0은 기본 해상도, 0.5는 절반 해상도 등)
    float ShadowResolutionScale = 0.0f;

    // 그림자가 객체 표면에 겹쳐 보이지 않도록 살짝 밀어내는 오프셋 값
    float ShadowBias = 0.0f;

    // 표면 기울기에 따라 동적으로 적용되는 추가 바이어스 (기울어진 면에서의 셀프섀도잉 방지)
    float ShadowSlopeBias = 0.0f;

    // 그림자 경계의 선명도를 조절하는 값 (값이 클수록 더 날카로운 그림자 경계)
    float ShadowSharpen = 0.0f;
public:
    void TickComponent(float DeltaTime) override;
    int CheckRayIntersection(FVector& rayOrigin, FVector& rayDirection, float& pfNearHitDistance) override;
    void InitializeComponent() override;
    void BeginPlay() override;
    void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    UObject* Duplicate() override;
    void DuplicateSubObjects(const UObject* Source) override;
    void PostDuplicate() override;

    
    std::unique_ptr<FActorComponentInfo> GetComponentInfo() override;
    void SaveComponentInfo(FActorComponentInfo& OutInfo) override;
    void LoadAndConstruct(const FActorComponentInfo& Info) override;
};
