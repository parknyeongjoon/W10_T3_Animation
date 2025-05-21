#include "AssetFactory.h"

UAssetFactory::UAssetFactory()
{
}

UAssetFactory::~UAssetFactory()
{
}

bool UAssetFactory::CanImport(const FString& filepath) const
{
    const FString ext = std::filesystem::path(filepath).extension().string();
    for (const FString& supported : SupportedExtensions)
    {
        if (supported == ext)
        {
            return true;
        }
    }
    return false;
}
