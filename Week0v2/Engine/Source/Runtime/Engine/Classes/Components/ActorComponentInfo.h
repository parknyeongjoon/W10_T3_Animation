#pragma once
#include "Container/String.h"
#include "Components/ActorComponent.h"
#include "Container/Array.h"
#include "Math/Vector.h"
#include "Math/Quat.h"
#include "Serialization/Archive.h"
#include <functional>
#include <memory>


// 액터 컴포넌트의 직렬화/역직렬화에 필요한 메타데이터 구조체
struct FActorComponentInfo
{
    DECLARE_ACTORCOMPONENT_INFO(FActorComponentInfo);
    
    FString InfoType;
    EComponentOrigin Origin;
    
    FGuid ComponentID;
    
    FString ComponentClass;
    FString ComponentName;
    FString ComponentOwner;

    bool bTickEnabled;
    bool bIsActive;
    bool bAutoActive;
    bool bIsRoot;

    
    FActorComponentInfo()
        : InfoType(TEXT("FActorComponentInfo")), Origin(EComponentOrigin::None), ComponentID(), ComponentClass(TEXT("")),
    bTickEnabled(true), bIsActive(false), bAutoActive(false), bIsRoot(false) 
    {}
    
    virtual ~FActorComponentInfo() = default;

    virtual void Serialize(FArchive& ar) const
    {
        ar << InfoType << ComponentClass << (int)Origin  << ComponentName << ComponentOwner << bTickEnabled << bIsActive << bAutoActive << bIsRoot;
        ar << ComponentID.A << ComponentID.B << ComponentID.C << ComponentID.D;
    }

    virtual void Deserialize(FArchive& ar)
    {
        int iOrigin  = 0;
        ar >> InfoType >> ComponentClass >> iOrigin >> ComponentName >> ComponentOwner >> bTickEnabled >> bIsActive >> bAutoActive>> bIsRoot;
        ar >> ComponentID.A >> ComponentID.B >> ComponentID.C >> ComponentID.D;
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