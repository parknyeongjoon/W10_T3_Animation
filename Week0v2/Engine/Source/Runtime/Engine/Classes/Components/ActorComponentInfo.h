#pragma once
#include "Container/String.h"
#include "Container/Array.h"
#include "Math/Vector.h"
#include "Serialization/Archive.h"
#include "Components/ActorComponent.h"
#include "Math/Quat.h"

// 액터 컴포넌트의 직렬화/역직렬화에 필요한 메타데이터 구조체
struct FActorComponentInfo
{
    FString Type;
    EComponentOrigin Origin;
    bool bIsRoot;

    // other의 데이터를 이 객체의 데이터로 복사합니다.
    virtual void Copy(FActorComponentInfo& Other)
    {
        Other.Type = Type;
        Other.Origin = Origin;
        Other.bIsRoot = bIsRoot;
    }

    virtual void Serialize(FArchive& ar) const
    {
        ar << Type << (int)Origin << bIsRoot;
    }

    virtual void Deserialize(FArchive& ar)
    {
        int iOrigin;
        ar >> Type >> iOrigin >> bIsRoot;
        Origin = static_cast<EComponentOrigin>(iOrigin);
    }
};