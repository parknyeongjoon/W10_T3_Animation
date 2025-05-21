#pragma once
#include <mutex>

#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

class UAssetFactory;
struct FAssetDescriptor;
class UAsset;

// 에셋 임포트 설정 구조체
struct ImportSettings
{
    bool bReimport = false;
    TMap<FString, FString> Options;
};

// “언제·어디서·어떤 설정으로” 여러 팩토리를 호출해 일괄 처리하고, 그 결과물을 AssetManager나 UI 등 후속 단계로 넘기는 상위 매니저 역할
class UAssetImporter : public UObject
{
    DECLARE_CLASS(UAssetImporter, UObject)
public:
    // 임포트 완료 콜백: (AssetDescriptor, AssetPtr)
    using ImportCallback = std::function<void(const FAssetDescriptor&, UAsset*)>;

    /** UAssetManager를 가져옵니다. */
    static UAssetImporter& Get();
    
    UAssetImporter();
    ~UAssetImporter() override;
    
    // 단일 파일 임포트
    void Import(const FString& InFilepath, ImportCallback InCallback);
    // 디렉터리 전체 임포트
    void ImportDirectory(const FString& InDir, const ImportSettings& InSettings, ImportCallback InCallback);

private:
    // 팩토리 검색 및 호출 
    UAssetFactory* FindFactory(const FString& filepath);
};
