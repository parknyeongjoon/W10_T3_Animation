#pragma once
#include "Container/String.h"
#include "Container/Array.h"
#include "Components/ActorComponentInfo.h"
#include "Serialization/Archive.h"
// Actor의 Deserialize에 필요한 메타데이터 구조체
struct FActorInfo
{
    FString Type;
    TArray<FString> InfoTypes;
    TArray<std::shared_ptr<FActorComponentInfo>> ComponentInfos;


    void Serialize(FArchive& ar)
    {
        ar << Type;
        InfoTypes.Empty();
        for (const auto& ComponentInfo : ComponentInfos)
        {
            InfoTypes.Add(ComponentInfo->InfoType);
        }

        // 1.타입 먼저 직렬화
        ar << InfoTypes;

        for (const auto& ComponentInfo : ComponentInfos)
        {
            // 컴포넌트 정보들은 각각 직렬화
            ComponentInfo->Serialize(ar);
        }
    }

    void Deserialize(FArchive& ar)
    {
        ar >> Type;

        ar >> InfoTypes;

        ComponentInfos.Empty();
        for (const FString& CompType : InfoTypes)
        {
            if (auto FactoryFunc = FActorComponentInfo::GetFactoryMap().Find(CompType))
            {
                auto NewCompInfo = (*FactoryFunc)();
                NewCompInfo->Deserialize(ar);
                ComponentInfos.Add(NewCompInfo);
            }
        }
    }
};