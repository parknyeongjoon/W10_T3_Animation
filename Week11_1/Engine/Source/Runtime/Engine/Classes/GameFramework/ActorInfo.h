#pragma once
#include "Components/ActorComponentInfo.h"
#include "Container/String.h"
#include "Container/Array.h"
// Actor의 Deserialize에 필요한 메타데이터 구조체
struct FActorInfo
{
    FString Type;
    TArray<FString> InfoTypes;
    TArray<std::unique_ptr<class FActorComponentInfo>> ComponentInfos;


    // // 기본 생성자
    FActorInfo() = default;
    // 기본 소멸자 (unique_ptr이 알아서 처리)
    ~FActorInfo() = default;
    //
    // // --- 복사 작업 삭제 ---
    FActorInfo(const FActorInfo&) = delete;
    FActorInfo& operator=(const FActorInfo&) = delete;
    //
    // // --- 이동 작업 허용 (선택 사항, 하지만 권장) ---
    FActorInfo(FActorInfo&&) = default; // 이동 생성자
    FActorInfo& operator=(FActorInfo&&) = default; // 이동 대입 연산자
    
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
                   ComponentInfos.Add(std::move(NewCompInfo));
               }
           }
       }
   }

};