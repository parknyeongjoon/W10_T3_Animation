#include "AssetRegistry.h"

#include "Asset.h"

UAssetRegistry::UAssetRegistry()
{
}

UAssetRegistry::~UAssetRegistry()
{
}

void UAssetRegistry::ScanDirectory(const FString& InDir)
{
    const std::filesystem::path baseDir(InDir);
    
    for (const auto& p : std::filesystem::recursive_directory_iterator(InDir))
    {
        if (!p.is_regular_file())
            continue;
        
        FAssetDescriptor desc;
        desc.AbsolutePath = p.path().string();
        // baseDir 기준 상대 경로 계산
        desc.RelativePath = std::filesystem::relative(p.path(), baseDir).string();
        desc.AssetName = FString(p.path().stem().string());
        desc.AssetExtension = p.path().extension().string();
        desc.Size         = std::filesystem::file_size(p.path());
        desc.CreateDate   = std::filesystem::last_write_time(p.path());
        desc.UpdateDate   = desc.CreateDate;
        RegisterDescriptor(desc);
    }
}

void UAssetRegistry::RegisterDescriptor(const FAssetDescriptor& InDesc)
{
    DescriptorMap[InDesc.AssetName] = InDesc;
}

bool UAssetRegistry::GetDescriptor(const FName& InName, FAssetDescriptor& OutDesc) const
{
    const FAssetDescriptor* it = DescriptorMap.Find(InName);
    if (it != nullptr)
    {
        OutDesc = *it;
        return true;
    }
    return false;
}
