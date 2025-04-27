#include "LightComponent.h"

ULightComponent::ULightComponent()
    : Super()
{
}

ULightComponent::ULightComponent(const ULightComponent& Other)
    : Super(Other)
    , ShadowResolutionScale(Other.ShadowResolutionScale)
    , ShadowBias(Other.ShadowBias)
    , ShadowSlopeBias(Other.ShadowSlopeBias)
    , ShadowSharpen(Other.ShadowSharpen)
{
}

ULightComponent::~ULightComponent()
{
}

void ULightComponent::UninitializeComponent()
{
    Super::UninitializeComponent();
}

void ULightComponent::OnComponentDestroyed()
{
    Super::OnComponentDestroyed();
}

void ULightComponent::DestroyComponent()
{
    Super::DestroyComponent();
}

void ULightComponent::OnRegister()
{
    Super::OnRegister();
}

void ULightComponent::OnUnregister()
{
    Super::OnUnregister();
}

void ULightComponent::TickComponent(float DeltaTime)
{
    Super::TickComponent(DeltaTime);
}

int ULightComponent::CheckRayIntersection(FVector& rayOrigin, FVector& rayDirection, float& pfNearHitDistance)
{
    return Super::CheckRayIntersection(rayOrigin, rayDirection, pfNearHitDistance);
}

void ULightComponent::InitializeComponent()
{
    Super::InitializeComponent();
}

void ULightComponent::BeginPlay()
{
    Super::BeginPlay();
}

void ULightComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
}

UObject* ULightComponent::Duplicate() const
{
    ULightComponentBase* NewComp = FObjectFactory::ConstructObjectFrom<ULightComponentBase>(this);
    NewComp->DuplicateSubObjects(this);
    NewComp->PostDuplicate();
    return NewComp;
}

void ULightComponent::DuplicateSubObjects(const UObject* Source)
{
    Super::DuplicateSubObjects(Source);
}

void ULightComponent::PostDuplicate()
{
    Super::PostDuplicate();
}

std::unique_ptr<FActorComponentInfo> ULightComponent::GetComponentInfo()
{
    auto Info = std::make_unique<FLightComponentInfo>();
    SaveComponentInfo(*Info);
    
    return Info;
}

void ULightComponent::SaveComponentInfo(FActorComponentInfo& OutInfo)
{
    FLightComponentInfo& Info = static_cast<FLightComponentInfo&>(OutInfo);
    Super::SaveComponentInfo(OutInfo);
    Info.ShadowResolutionScale = ShadowResolutionScale;
    Info.ShadowBias = ShadowBias;
    Info.ShadowSlopeBias = ShadowSlopeBias;
    Info.ShadowSharpen = ShadowSharpen;
    
}

void ULightComponent::LoadAndConstruct(const FActorComponentInfo& Info)
{
    Super::LoadAndConstruct(Info);
    const FLightComponentInfo& LightInfo = static_cast<const FLightComponentInfo&>(Info);
    ShadowResolutionScale = LightInfo.ShadowResolutionScale;
    ShadowBias = LightInfo.ShadowBias;
    ShadowSlopeBias = LightInfo.ShadowSlopeBias;
    ShadowSharpen = LightInfo.ShadowSharpen;
}
