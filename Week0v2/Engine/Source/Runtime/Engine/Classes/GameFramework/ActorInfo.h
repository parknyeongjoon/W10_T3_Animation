#pragma once
#include "Container/String.h"
#include "Container/Array.h"
#include "Components/ActorComponentInfo.h"
#include "Serialization/Archive.h"
// Actor의 Deserialize에 필요한 메타데이터 구조체
struct FActorInfo
{
    FString Type;
    TArray<FActorComponentInfo> ComponentInfos;

    void Serialize(FArchive& ar) const
    {
        ar << Type << ComponentInfos;
    }

    void Deserialize(FArchive& ar)
    {
        ar >> Type >> ComponentInfos;
    }
};