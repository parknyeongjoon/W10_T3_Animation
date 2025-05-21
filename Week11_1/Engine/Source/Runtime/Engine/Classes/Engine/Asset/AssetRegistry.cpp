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
    namespace fs = std::filesystem;
    // ① 기준 디렉토리를 절대경로로 변환
    fs::path baseDir = fs::absolute(fs::path(GetData(InDir)));
    
    for (const auto& entry  : fs::recursive_directory_iterator(baseDir))
    {
        if (!entry.is_regular_file())
            continue;

        fs::path filePath = entry.path();
        FAssetDescriptor desc;

        // ② 절대경로 (OS 구분자와 상관없이)
        desc.AbsolutePath = FString(filePath.generic_wstring().c_str());

        // ③ baseDir 대비 상대경로 (항상 '/' 로만 구성)
        fs::path rel = fs::relative(filePath, baseDir);
        desc.RelativePath = FString(rel.generic_string().c_str());

        desc.AssetName      = FString(rel.stem().string().c_str());
        desc.AssetExtension = rel.extension().string();
        desc.Size           = fs::file_size(filePath);
        desc.CreateDate     = fs::last_write_time(filePath);
        desc.UpdateDate     = desc.CreateDate;

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
