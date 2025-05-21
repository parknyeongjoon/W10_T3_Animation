#include "AssetImporter.h"

#include "Asset.h"
#include "AssetFactory.h"
#include "AssetManager.h"
#include "Engine/FEditorStateManager.h"
#include "Serialization/Serializer.h"

UAssetImporter& UAssetImporter::Get()
{
    if (UAssetImporter* Singleton = GEngine->AssetImporter)
    {
        return *Singleton;
    }
    else
    {
        UE_LOG(LogLevel::Error, "Cannot use AssetManager if no AssetManagerClassName is defined!");
        assert(0);
        return *new UAssetImporter; // never calls this
    }
}

UAssetImporter::UAssetImporter()
{
}

UAssetImporter::~UAssetImporter()
{
}

void UAssetImporter::Import(const FString& InFilepath, const ImportSettings& InSettings, ImportCallback InCallback)
{
    auto factory = FindFactory(InFilepath);
    if (!factory) return;

    // 디스크 읽기→Descriptor 생성
    FAssetDescriptor desc;
    std::filesystem::path filePath(InFilepath);

    // 에셋 식별자
    desc.AssetName = FName(filePath.stem().string());
    
    // 경로 정보
    desc.AbsolutePath  = filePath.string();
    desc.RelativePath  = std::filesystem::relative(filePath, std::filesystem::current_path()).string();
    
    // 확장자
    desc.AssetExtension = filePath.extension().string();

    // 메타데이터
    desc.Size       = std::filesystem::file_size(filePath);
    desc.CreateDate = std::filesystem::last_write_time(filePath);
    desc.UpdateDate = desc.CreateDate; 

    UAsset* asset;
    if (desc.AssetExtension == ".uasset")
    {
        // .uasset: Serializer로 직접 로드
        UObject* root = Serializer::LoadFromFile(filePath);
        asset = Cast<UAsset>(root);
    }
    else
    {
        // 그 외: 팩토리 임포트
        asset = factory->ImportFromFile(InFilepath);
    }

    if (asset)
    {
        UAssetManager::Get().Store(desc.AssetName, asset);
    }
    
    if (InCallback)
    {
        InCallback(desc, asset);
    }
}

void UAssetImporter::ImportDirectory(const FString& InDir, const ImportSettings& InSettings, ImportCallback InCallback)
{
    for (auto& p : std::filesystem::recursive_directory_iterator(InDir))
    {
        if (!p.is_regular_file()) continue;
        Import(p.path().string(), InSettings, InCallback);
    }
}

UAssetFactory* UAssetImporter::FindFactory(const FString& filepath)
{
    return UAssetManager::Get().FindFactoryForFile(filepath);
}
