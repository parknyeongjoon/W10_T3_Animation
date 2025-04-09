#include "LightComponent.h"
#include "UBillboardComponent.h"
#include "Math/JungleMath.h"
#include "UnrealEd/PrimitiveBatch.h"
#include "UObject/ObjectFactory.h"
#include "CoreUObject/UObject/Casts.h"
#include "GameFramework/Actor.h"

ULightComponentBase::ULightComponentBase()
{
    // FString name = "SpotLight";
    // SetName(name);
    color = { 1,1,1,1 };
}

ULightComponentBase::ULightComponentBase(const ULightComponentBase& Other)
{
}

ULightComponentBase::~ULightComponentBase()
{
    //delete texture2D;
}

void ULightComponentBase::SetColor(FVector4 newColor)
{
    color = newColor;
}

FVector4 ULightComponentBase::GetColor() const
{
    return color;
}

UObject* ULightComponentBase::Duplicate() const
{
    ULightComponentBase* NewComp = FObjectFactory::ConstructObjectFrom<ULightComponentBase>(this);
    //NewComp->InitializeLight();
    NewComp->DuplicateSubObjects(this);
    NewComp->PostDuplicate();
    return NewComp;
}

void ULightComponentBase::DuplicateSubObjects(const UObject* Source)
{
    // 복사할 것?
    Super::DuplicateSubObjects(Source);

    ULightComponentBase* SourceComp = Cast<ULightComponentBase>(Source);
    if (SourceComp)
    {
        SetLocation(SourceComp->GetLocalLocation());
        color = SourceComp->color;
        Intensity = SourceComp->Intensity;
        AABB = SourceComp->AABB;
    }

}

void ULightComponentBase::PostDuplicate()
{
}

std::shared_ptr<FActorComponentInfo> ULightComponentBase::GetActorComponentInfo()
{
    std::shared_ptr<FLightComponentInfo> Info = std::make_shared<FLightComponentInfo>();
    Super::GetActorComponentInfo()->Copy(*Info);

    Info->Color = color;
    Info->Intensity = Intensity;
    Info->AABB = AABB;

    return Info;
}

void ULightComponentBase::LoadAndConstruct(const FActorComponentInfo& Info)
{
    Super::LoadAndConstruct(Info);
    const FLightComponentInfo& LightInfo = static_cast<const FLightComponentInfo&>(Info);
    color = LightInfo.Color;
    Intensity = LightInfo.Intensity;
}

void ULightComponentBase::InitializeLight()
{
    AABB.max = { 1.f,1.f,0.1f };
    AABB.min = { -1.f,-1.f,-0.1f };
}

void ULightComponentBase::TickComponent(float DeltaTime)
{
    Super::TickComponent(DeltaTime);


}

int ULightComponentBase::CheckRayIntersection(FVector& rayOrigin, FVector& rayDirection, float& pfNearHitDistance)
{
    bool res =AABB.Intersect(rayOrigin, rayDirection, pfNearHitDistance);
    return res;
}

void ULightComponentBase::InitializeComponent()
{

}

void ULightComponentBase::BeginPlay()
{
}

void ULightComponentBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
}


