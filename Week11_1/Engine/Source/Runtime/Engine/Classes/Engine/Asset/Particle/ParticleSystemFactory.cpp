#include "ParticleSystemFactory.h"

#include "Particles/ParticleSystem.h"

UParticleSystemFactory::~UParticleSystemFactory()
{
}

bool UParticleSystemFactory::CanImport(const FString& filepath) const
{
    return Super::CanImport(filepath);
}

UAsset* UParticleSystemFactory::ImportFromFile(const FString& InFilePath)
{
    UParticleSystem* NewParticleSystem = FObjectFactory::ConstructObject<UParticleSystem>(nullptr);
    if (!NewParticleSystem->LoadFromFile(InFilePath))
    {
        return nullptr;
    }

    NewParticleSystem->GetDescriptor().AbsolutePath = InFilePath;
    NewParticleSystem->GetDescriptor().RelativePath = std::filesystem::relative(InFilePath, "Content").string();

    return NewParticleSystem;
}

UAsset* UParticleSystemFactory::CreateNew(const FAssetDescriptor& desc)
{
    UParticleSystem* NewParticleSystem = FObjectFactory::ConstructObject<UParticleSystem>(nullptr);
    return NewParticleSystem;
}
