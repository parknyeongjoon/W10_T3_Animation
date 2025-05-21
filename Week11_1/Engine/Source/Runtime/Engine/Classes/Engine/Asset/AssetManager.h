#pragma once
#include <mutex>

#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "Asset.h"
#include "AssetFactory.h"
#include "AssetRegistry.h"
#include "Serialization/Serializer.h"
#include "UObject/Casts.h"

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

    void Initalize();
    
    void InitAssetManager();

    const TMap<FName, FAssetInfo>& GetAssetRegistry() const;

    // 팩토리 등록/해제
    void RegisterFactory(UAssetFactory* InFactory);
    void UnregisterFactory(UAssetFactory* InFactory);

    // ClassType을 템플릿으로 받아서 원하는 타입으로 리턴
    template<typename T>
    T* Load(const FString& Path);
    
    // 에셋 로드 (캐시 적용)
    //* Load(const FString& InFilepath);
    
    // 캐시 등록
    void Store(const FName& InName, UAsset* InAsset);
    
    // 에셋 조회
    template<typename T>
    T* Get(const FString& InName);
    
    // 에셋 언로드
    void Unload(const FString& InName);

    // 루트 오브젝트를 디스크에 .uasset/.umap 형태로 저장
    bool SaveAsset(UObject* Root, const FString& Path);

    // 디스크에서 읽어들여 해당 UClass 타입의 오브젝트를 반환
    UObject* LoadAsset(const FString& Path, UClass* ClassType);

    template<typename T>
    TMap<FName, T*> GetLoadedAssetsByType() const
    {
        TMap<FName, T*> Result;
        for (const auto& Pair : LoadedAssets)
        {
            if (T* Asset = Cast<T>(Pair.Value))
            {
                Result.Add(Pair.Key, Asset);
            }
        }
        return Result;
    }

public:
    void LoadObjFiles();
    UAssetFactory* FindFactoryForFile(const FString& filepath);

private:
    UAssetRegistry* Registry;
    TArray<UAssetFactory*> Factories;
    TMap<FName, UAsset*> LoadedAssets;

    mutable std::mutex Mutex;  // 스레드 안전 보장
};

template <typename T>
T* UAssetManager::Load(const FString& Path)
{
    namespace fs = std::filesystem;
    fs::path     fsPath(Path.ToWideString());
    FString      Name = fsPath.stem().string();

    // 1) 캐시 조회
    {
        std::lock_guard<std::mutex> lock(Mutex);
        if (UAsset** Cached = LoadedAssets.Find(Name))
        {
            return Cast<T>(*Cached);
        }
    }

    // 2) 파일 존재 및 크기 확인
    if (!fs::exists(fsPath) || fs::file_size(fsPath) < sizeof(uint32))
        return nullptr;

    T* Asset = nullptr;
    const auto ext = fsPath.extension().string();

    // 3) .ttalkak → Serializer 로드
    if (ext == ".ttalkak")
    {
        UObject* Raw = Serializer::LoadFromFile(fsPath);
        Asset = Cast<T>(Raw);
    }
    // 4) 그 외 포맷 → 팩토리 임포트
    else
    {
        for (UAssetFactory* F : Factories)
        {
            if (F->CanImport(Path))
            {
                Asset = Cast<T>(F->ImportFromFile(Path));
                break;
            }
        }
    }

    // 5) 캐시에 저장
    if (Asset)
    {
        std::lock_guard<std::mutex> lock(Mutex);
        LoadedAssets.Add(Name, Asset);
    }
    return Asset;
}

template <typename T>
T* UAssetManager::Get(const FString& InName)
{
    // 1) Registry에서 Descriptor만 꺼내고
    FAssetDescriptor desc;
    if (!Registry || !Registry->GetDescriptor(InName, desc))
        return nullptr;

    // 2) Load 에 모든 캐시 검사+로드 로직 위임
    return Load<T>(desc.AbsolutePath);
}
