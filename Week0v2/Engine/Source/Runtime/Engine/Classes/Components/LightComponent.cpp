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
        color = SourceComp->color;
        Intensity = SourceComp->Intensity;
        AABB = SourceComp->AABB;
        texture2D = new UBillboardComponent();
        texture2D->SetTexture(L"Assets/Texture/spotLight.png");
        texture2D->InitializeComponent();
    }

}

void ULightComponentBase::PostDuplicate()
{
}

void ULightComponentBase::InitializeLight()
{
    texture2D = GetOwner()->AddComponent<UBillboardComponent>();
    texture2D->SetTexture(L"Assets/Texture/spotLight.png");

    AABB.max = { 1.f,1.f,0.1f };
    AABB.min = { -1.f,-1.f,-0.1f };
    color = { 1,1,1,1 };
}

void ULightComponentBase::TickComponent(float DeltaTime)
{
    Super::TickComponent(DeltaTime);

    texture2D->TickComponent(DeltaTime);
    texture2D->SetLocation(GetWorldLocation());

}

int ULightComponentBase::CheckRayIntersection(FVector& rayOrigin, FVector& rayDirection, float& pfNearHitDistance)
{
    bool res =AABB.Intersect(rayOrigin, rayDirection, pfNearHitDistance);
    return res;
}

void ULightComponentBase::InitializeComponent()
{
    InitializeLight();
}

void ULightComponentBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{

}

