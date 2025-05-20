#pragma once
#include <mutex>

#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "Asset.h"

class UAssetRegistry;
class UAssetFactory;

struct FAssetInfo
{
    FName AssetName;      // Asset의 이름
    FName PackagePath;    // Asset의 패키지 경로
    EAssetType AssetType; // Asset의 타입
    uint32 Size;          // Asset의 크기 (바이트 단위)
};

struct FAssetRegistry
{
    TMap<FName, FAssetInfo> PathNameToAssetInfo;
};

class UAssetManager : public UObject
{
    DECLARE_CLASS(UAssetManager, UObject)

private:
    std::unique_ptr<FAssetRegistry> AssetRegistry;

public:
    UAssetManager() = default;

    static bool IsInitialized();

    /** UAssetManager를 가져옵니다. */
    static UAssetManager& Get();

    /** UAssetManager가 존재하면 가져오고, 없으면 nullptr를 반환합니다. */
    static UAssetManager* GetIfInitialized();

    void Initalize(UAssetRegistry* InRegistry);
    
    void InitAssetManager();

    const TMap<FName, FAssetInfo>& GetAssetRegistry() const;

    // 팩토리 등록/해제
    void RegisterFactory(UAssetFactory* InFactory);
    void UnregisterFactory(UAssetFactory* InFactory);

    // 에셋 로드 (캐시 적용)
    UAsset* Load(const FString& InFilepath);
    // 캐시 등록
    void Store(const FName& InName, UAsset* InAsset);
    // 에셋 조회
    UAsset* Get(const FString& InName);
    // 에셋 언로드
    void Unload(const FString& InName);

    // 루트 오브젝트를 디스크에 .uasset/.umap 형태로 저장
    static bool SaveAsset(UObject* Root, const FString& Path);

    // 디스크에서 읽어들여 해당 UClass 타입의 오브젝트를 반환
    static UObject* LoadAsset(const FString& Path, UClass* ClassType);

public:
    void LoadObjFiles();
    UAssetFactory* FindFactoryForFile(const FString& filepath);

private:
    UAssetRegistry* Registry;
    TArray<UAssetFactory*> Factories;
    TMap<FName, UAsset*> LoadedAssets;

    mutable std::mutex Mutex;  // 스레드 안전 보장
};