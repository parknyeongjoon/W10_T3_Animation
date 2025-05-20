#pragma once
#include "Engine/Asset/AssetFactory.h"

class UParticleSystemFactory : public UAssetFactory
{
    DECLARE_CLASS(UParticleSystemFactory, UAssetFactory)
public:
    UParticleSystemFactory()
    {
        // .uparticles 파일을 파티클 시스템 에셋으로 인식
        SetSupportedExtensions({".uparticles"});
        SetPriority(50);
    }
    ~UParticleSystemFactory() override;

    // 파일 임포트 가능 여부 검사
    bool CanImport(const FString& filepath) const override;

    // 파일로부터 에셋 생성
    UAsset* ImportFromFile(const FString& InFilePath) override;

    // 빈 에셋 신규 생성
    UAsset* CreateNew(const FAssetDescriptor& desc) override;
};
