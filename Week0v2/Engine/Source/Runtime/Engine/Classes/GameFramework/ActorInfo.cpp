#include "ActorInfo.h"
#include "Components/ActorComponentInfo.h"
#include "Components/ActorComponent.h"
#include "Serialization/Archive.h"


// void FActorInfo::Serialize(FArchive& ar)
// {
//     ar << Type;
//     InfoTypes.Empty();
//     for (const auto& ComponentInfo : ComponentInfos)
//     {
//         InfoTypes.Add(ComponentInfo->InfoType);
//     }
//
//     // 1.타입 먼저 직렬화
//     ar << InfoTypes;
//
//     for (const auto& ComponentInfo : ComponentInfos)
//     {
//         // 컴포넌트 정보들은 각각 직렬화
//         ComponentInfo->Serialize(ar);
//     }
// }

// void FActorInfo::Deserialize(FArchive& ar)
// {
//     {
//         ar >> Type;
//
//         ar >> InfoTypes;
//
//         ComponentInfos.Empty();
//         for (const FString& CompType : InfoTypes)
//         {
//             if (auto FactoryFunc = FActorComponentInfo::GetFactoryMap().Find(CompType))
//             {
//                 auto NewCompInfo = (*FactoryFunc)();
//                 NewCompInfo->Deserialize(ar);
//                 ComponentInfos.Add(std::move(NewCompInfo));
//             }
//         }
//     }
// }
