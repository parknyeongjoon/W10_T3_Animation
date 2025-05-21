#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

struct FAssetDescriptor;

class UAssetRegistry : public UObject
{
    DECLARE_CLASS(UAssetRegistry, UObject)
public:
    UAssetRegistry();
    virtual ~UAssetRegistry();
    
    /**
      * 디렉터리를 스캔하여 메타데이터 등록
      * @param dir : 실행 디렉터리 기준 상대 경로(예: "Assets/Content")
      */
    void ScanDirectory(const FString& InDir = TEXT("Contents"));
    void RegisterDescriptor(const FAssetDescriptor& InDesc);

    // 등록된 에셋 검색
    bool GetDescriptor(const FName& InName, FAssetDescriptor& OutDesc) const;

private:
    TMap<FName, FAssetDescriptor> DescriptorMap;
};
