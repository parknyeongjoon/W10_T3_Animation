#include "DirectionalLightComponent.h"
#include "UObject/ObjectFactory.h"
#include "CoreUObject/UObject/Casts.h"

UDirectionalLightComponent::UDirectionalLightComponent()
{
}

UDirectionalLightComponent::UDirectionalLightComponent(const UDirectionalLightComponent& Other)
{
}

//void UDirectionalLightComponent::SetDirection(FVector _newDir)
//{
//    //잘 안됨
//    FVector Axis = Direction.Cross(_newDir).Normalize();
//    float Angle = acosf(Direction.Normalize().Dot(_newDir.Normalize()));
//    GetOwner()->GetRootComponent()->SetRelativeQuat(FQuat::FromAxisAngle(Axis, Angle));
//    Direction = _newDir;
//}

UObject* UDirectionalLightComponent::Duplicate() const
{
    UDirectionalLightComponent* NewComp = FObjectFactory::ConstructObjectFrom<UDirectionalLightComponent>(this);
    NewComp->DuplicateSubObjects(this);
    NewComp->PostDuplicate();

    return NewComp;
}

void UDirectionalLightComponent::DuplicateSubObjects(const UObject* Source)
{
    Super::DuplicateSubObjects(Source);
    // 여기서 복사할 것? 방향뿐임
    UDirectionalLightComponent* SourceComp = Cast<UDirectionalLightComponent>(Source);
    SourceComp->Direction = Direction;
}

void UDirectionalLightComponent::PostDuplicate()
{
}

std::shared_ptr<FActorComponentInfo> UDirectionalLightComponent::GetActorComponentInfo()
{
    std::shared_ptr<FDirectionalLightComponentInfo> Info = std::make_shared<FDirectionalLightComponentInfo>();
    Super::GetActorComponentInfo()->Copy(*Info);
    Info->Direction = Direction;
    return Info;
}

void UDirectionalLightComponent::LoadAndConstruct(const FActorComponentInfo& Info)
{
    Super::LoadAndConstruct(Info);
    const FDirectionalLightComponentInfo& DirectionalLightInfo = static_cast<const FDirectionalLightComponentInfo&>(Info);
    Direction = DirectionalLightInfo.Direction;
}
