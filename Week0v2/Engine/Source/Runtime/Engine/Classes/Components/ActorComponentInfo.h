#pragma once
#include "Container/String.h"
#include "Container/Array.h"
#include "Math/Vector.h"
#include "Serialization/Archive.h"
#include "Components/ActorComponent.h"
#include "Math/Quat.h"
#include <functional>
#include <memory>


// 액터 컴포넌트의 직렬화/역직렬화에 필요한 메타데이터 구조체
struct FActorComponentInfo
{
    DECLARE_ACTORCOMPONENT_INFO(FActorComponentInfo);
    
    FString InfoType;
    EComponentOrigin Origin;
    
    FString ComponentClass;
    FString ComponentName;
    FString ComponentOwner;

    bool bTickEnabled;
    bool bIsActive;
    bool bAutoActive;
    bool bIsRoot;
    
    
    FActorComponentInfo()
        : InfoType(TEXT("FActorComponentInfo")), ComponentClass(TEXT("")), Origin(EComponentOrigin::None), bIsRoot(false),
    bTickEnabled(true), bIsActive(false), bAutoActive(false)
    {}
    
    virtual ~FActorComponentInfo() = default;

    virtual void Serialize(FArchive& ar) const
    {
        ar << InfoType << ComponentClass << (int)Origin << ComponentName << ComponentOwner << bIsRoot;
    }

    virtual void Deserialize(FArchive& ar)
    {
        int iOrigin;
        ar >> InfoType >> ComponentClass >> iOrigin >> ComponentName >> ComponentOwner >> bIsRoot;
        Origin = static_cast<EComponentOrigin>(iOrigin);
    }

    void SerializeType(FArchive& ar) const 
    {
        ar << InfoType;
    }

    void DeserializeType(FArchive& ar)
    {
        ar >> InfoType;
    }

    using BaseFactoryFunc = std::function<std::unique_ptr<FActorComponentInfo>()>;

    inline static TMap<FString, BaseFactoryFunc>& GetFactoryMap()
    {
        static TMap<FString, BaseFactoryFunc> FactoryMap;
        return FactoryMap;
    }
};

inline FArchive& operator<<(FArchive& ar, const FActorComponentInfo& Info)
{
    Info.Serialize(ar);
    return ar;
}

inline FArchive& operator>>(FArchive& ar, FActorComponentInfo& Info)
{
    Info.Deserialize(ar);
    return ar;
}