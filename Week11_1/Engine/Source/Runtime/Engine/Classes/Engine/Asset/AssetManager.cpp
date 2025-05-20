#include "AssetManager.h"

#include <filesystem>

#include "AssetFactory.h"
#include "AssetRegistry.h"
#include "Engine/FLoaderOBJ.h"
#include "Particles/ParticleSystem.h"
#include "Serialization/MemoryArchive.h"
#include "UObject/Casts.h"

extern UEngine* GEngine;

bool UAssetManager::IsInitialized()
{
    return GEngine && GEngine->AssetManager;
}

UAssetManager& UAssetManager::Get()
{
    if (UAssetManager* Singleton = GEngine->AssetManager)
    {
        return *Singleton;
    }
    else
    {
        UE_LOG(LogLevel::Error, "Cannot use AssetManager if no AssetManagerClassName is defined!");
        assert(0);
        return *new UAssetManager; // never calls this
    }
}

UAssetManager* UAssetManager::GetIfInitialized()
{
    return GEngine ? GEngine->AssetManager : nullptr;
}

void UAssetManager::Initalize(UAssetRegistry* InRegistry)
{
    Registry = FObjectFactory::ConstructObject<UAssetRegistry>(this);
    Registry->ScanDirectory();
}

void UAssetManager::InitAssetManager()
{
    AssetRegistry = std::make_unique<FAssetRegistry>();

    LoadObjFiles();
}

const TMap<FName, FAssetInfo>& UAssetManager::GetAssetRegistry() const
{
    return AssetRegistry->PathNameToAssetInfo;
}

void UAssetManager::RegisterFactory(UAssetFactory* InFactory)
{
    std::lock_guard<std::mutex> lock(Mutex);
    Factories.Add(InFactory);
    std::sort(Factories.begin(), Factories.end(),
        [](const auto& A, const auto& B)
        {
            return A->GetPriority() > B->GetPriority();
        }
    );
}

void UAssetManager::UnregisterFactory(UAssetFactory* InFactory)
{
    std::lock_guard<std::mutex> lock(Mutex);
    Factories.Erase(
        std::remove(Factories.begin(), Factories.end(), InFactory),
        Factories.end()
    );
}

UAsset* UAssetManager::Load(const FString& InFilepath)
{
    FString Name = std::filesystem::path(InFilepath).stem().string();

    // --- 여기서만 캐시 검사 ---
    {
        std::lock_guard<std::mutex> lock(Mutex);
        UAsset** it = LoadedAssets.Find(Name);
        if (it != nullptr)
            return *it;
    }
    
    // 2) 파일 존재 여부 확인
    if (!std::filesystem::exists(InFilepath))
        return nullptr;

    // 2) 파일 크기 검사 (최소 4바이트 만큼은 있어야 NameLen을 읽을 수 있음)
    auto sz = std::filesystem::file_size(InFilepath);
    if (sz < sizeof(uint32))
        return nullptr;

    UAsset* Asset = nullptr;
    const auto ext = std::filesystem::path(InFilepath).extension().string();

    // // 3) 패키지(.uasset) 직접 로드
    // if (ext == ".uasset")
    // {
    //     FArchiveReader Reader(GetData(InFilepath));
    //
    //     // 2) Reader 열림 여부 확인
    //     if (!Reader.IsOpen())
    //     {
    //         return nullptr;
    //     }
    //     
    //     // LoadPackage 시작부에서
    //     uint32 NameLen = 0;
    //     Reader << NameLen;
    //
    //     // 클래스 이름(문자열)만큼의 데이터가 남아 있는지 확인
    //     if (sz < sizeof(uint32) + NameLen * sizeof(TCHAR))
    //         return nullptr;
    //
    //     FString ClassName;
    //     ClassName.Resize(NameLen);
    //     Reader.SerializeRaw(GetData(ClassName), NameLen * sizeof(TCHAR));
    //
    //     // 3) 로드 진행
    //     UObject* RawObj = FObjectFactory::ConstructObject(UClass::FindClass(ClassName), nullptr);
    //     RawObj->Serialize(Reader);
    //     Asset = static_cast<UAsset*>(RawObj);
    // }
    //else
    {
        // 4) 외부 포맷은 등록된 팩토리로 임포트
        for (UAssetFactory* Factory : Factories)
        {
            if (Factory->CanImport(InFilepath))
            {
                Asset = Factory->ImportFromFile(InFilepath);
                break;
            }
        }
    }

    // 로드 성공 시 캐시에 저장
    if (Asset)
    {
        std::lock_guard<std::mutex> lock(Mutex);
        LoadedAssets[Name] = Asset;
    }
    return Asset;
}

void UAssetManager::Store(const FName& InName, UAsset* InAsset)
{
    if (InAsset)
    {
        std::lock_guard<std::mutex> lock(Mutex);
        LoadedAssets[InName] = InAsset;
    }
}

UAsset* UAssetManager::Get(const FString& InName)
{
    // 1) Registry에서 Descriptor만 꺼내고
    FAssetDescriptor desc;
    if (!Registry || !Registry->GetDescriptor(InName, desc))
        return nullptr;

    // 2) Load 에 모든 캐시 검사+로드 로직 위임
    return Load(desc.RelativePath);
}

void UAssetManager::Unload(const FString& InName)
{
    std::lock_guard<std::mutex> lock(Mutex);
    LoadedAssets.Remove(InName);
}

bool UAssetManager::SaveAsset(UObject* Root, const FString& Path)
{
    // 1) 상위 디렉토리 확인 및 생성
    const std::filesystem::path fsPath(Path);
    const std::filesystem::path dir = fsPath.parent_path();
    if (!dir.empty() && !std::filesystem::exists(dir))
    {
        std::filesystem::create_directories(dir);
    }
    
    // FArchiveWriter Writer(GetData(Path));
    // if (!Writer.IsOpen())
    // {
    //     return false;
    // }
    //
    // FString ClassName = Root->GetClass()->GetName();
    // // 1) 클래스 이름 길이
    // uint32 NameLen = ClassName.Len();
    // Writer << NameLen;
    // // 2) 문자 데이터
    // Writer.SerializeRaw(GetData(ClassName), NameLen * sizeof(TCHAR));
    //
    // Root->Serialize(Writer);
    return true;
}

UObject* UAssetManager::LoadAsset(const FString& Path, UClass* ClassType)
{
    // // 1) 파일 존재 여부 확인
    // if (!std::filesystem::exists(Path))
    // {
    //     // 처리: nullptr 반환 혹은 로그 출력 등
    //     return nullptr;
    // }
    //
    // // 2) Reader 열림 여부 확인
    // FMemoryReader2 Reader(GetData(Path));
    // if (!Reader.IsOpen())
    // {
    //     return nullptr;
    // }
    //
    // // 3) 로드 진행
    // UObject* Asset = FObjectFactory::ConstructObject(ClassType, nullptr);
    //
    // // LoadPackage 시작부에서
    // uint32 NameLen = 0;
    // Reader << NameLen;
    // FString ClassName;
    // ClassName.Resize(NameLen);
    // Reader.SerializeRaw(GetData(ClassName), NameLen * sizeof(TCHAR));
    //
    // Asset->Serialize(Reader);
    //return Asset;
    return nullptr;
}

void UAssetManager::LoadObjFiles()
{
    const std::string BasePathName = "Contents/";

    // Obj 파일 로드
	
    for (const auto& Entry : std::filesystem::recursive_directory_iterator(BasePathName))
    {
        if (Entry.is_regular_file() && Entry.path().extension() == ".obj")
        {
            FAssetInfo NewAssetInfo;
            NewAssetInfo.AssetName = FName(Entry.path().filename().string());
            NewAssetInfo.PackagePath = FName(Entry.path().parent_path().string());
            NewAssetInfo.AssetType = EAssetType::StaticMesh; // obj 파일은 무조건 StaticMesh
            NewAssetInfo.Size = static_cast<uint32>(std::filesystem::file_size(Entry.path()));
            
            AssetRegistry->PathNameToAssetInfo.Add(NewAssetInfo.AssetName, NewAssetInfo);
            
            FString MeshName = NewAssetInfo.PackagePath.ToString() + "/" + NewAssetInfo.AssetName.ToString();
            FManagerOBJ::CreateStaticMesh(MeshName);
            // ObjFileNames.push_back(UGTLStringLibrary::StringToWString(Entry.path().string()));
            // FObjManager::LoadObjStaticMeshAsset(UGTLStringLibrary::StringToWString(Entry.path().string()));
        }

        if (Entry.is_regular_file() && Entry.path().extension() == ".csv")
        {
            FAssetInfo NewAssetInfo;
            NewAssetInfo.AssetName = FName(Entry.path().filename().string());
            NewAssetInfo.PackagePath = FName(Entry.path().parent_path().string());
            NewAssetInfo.AssetType = EAssetType::Curve;
            NewAssetInfo.Size = static_cast<uint32>(std::filesystem::file_size(Entry.path()));
            
            AssetRegistry->PathNameToAssetInfo.Add(NewAssetInfo.AssetName, NewAssetInfo);
            
            // ObjFileNames.push_back(UGTLStringLibrary::StringToWString(Entry.path().string()));
            // FObjManager::LoadObjStaticMeshAsset(UGTLStringLibrary::StringToWString(Entry.path().string()));
        }
    }
}

UAssetFactory* UAssetManager::FindFactoryForFile(const FString& filepath)
{
    std::string ext = std::filesystem::path(filepath).extension().string();
    std::lock_guard<std::mutex> lock(Mutex);
    for (const auto& factory : Factories)
    {
        // CanImport 내부에서 확장자 검사 등을 수행하도록 구현되어 있어야 함
        if (factory->CanImport(filepath))
        {
            return factory;
        }
    }
    return nullptr;
}
