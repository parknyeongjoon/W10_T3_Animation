#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

struct FAssetDescriptor;
class UAsset;

// “어떻게” 에셋을 만들어낼지(파일 파싱/객체 생성)를 책임
class UAssetFactory : public UObject
{
    DECLARE_ABSTRACT_CLASS(UAssetFactory, UObject)
public:
    UAssetFactory();
    virtual ~UAssetFactory();
    
    // 이 팩토리가 처리할 수 있는 파일 확장자 목록
    const TArray<FString>& GetSupportedExtensions() const { return SupportedExtensions; }
    void SetSupportedExtensions(const TArray<FString>& exts) { SupportedExtensions = exts; }

    // 높은 값일수록 우선순위 높음
    int GetPriority() const { return ImportPriority; }
    void SetPriority(int p) { ImportPriority = p; }

    // 파일 임포트 가능 여부
    virtual bool CanImport(const FString& filepath) const;
    // 파일로부터 에셋 생성
    virtual UAsset* ImportFromFile(const FString& filepath) = 0;
    // 빈 에셋 생성(예: 신규 생성)
    virtual UAsset* CreateNew(const FAssetDescriptor& desc) = 0;

protected:
    TArray<FString> SupportedExtensions;
    int ImportPriority = 100;
};
