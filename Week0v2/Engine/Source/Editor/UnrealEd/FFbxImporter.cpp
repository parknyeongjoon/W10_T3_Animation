#include "FFbxImporter.h"

#include "fbxsdk.h"

#include "Rendering/SkeletalMeshRenderData.h"

bool FFbxImporter::ParseOBJ(const FString& FilePath, FbxSceneInfo& OutFbxSceneInfo)
{
    FbxManager* FbxManager = FbxManager::Create();
    FbxIOSettings* FbxIOSetting = FbxIOSettings::Create(FbxManager, IOSROOT);
    FbxManager->SetIOSettings(FbxIOSetting);
    FbxScene* FbxScene = FbxScene::Create(FbxManager, "myScene");


    FbxImporter* Importer = FbxImporter::Create(FbxManager, "");
    
    // Try to load the FBX file
    if (!Importer->Initialize(GetData(FilePath), -1, FbxManager->GetIOSettings())) {
        MessageBox(nullptr, L"Error: Unable to initialize the FBX importer", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    // Import the scene from the file
    if (!Importer->Import(FbxScene)) {
        MessageBox(nullptr, L"Error: Unable to import the FBX scene.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    FbxIOSetting->Destroy();
    FbxManager->Destroy();
    Importer->Destroy();


    FbxScene->Destroy();    
}

FSkeletalMeshRenderData* FFbxManager::CreateSkeletalMeshAsset(const FString& FilePath)
{
}

FSkeletalMeshRenderData* FFbxManager::GetSkeletalMeshAsset(const FString& FilePath)
{
    FSkeletalMeshRenderData* SkeletonMeshRenderData;
    return nullptr;
}

bool FFbxManager::LoadSkeletalMesh(const FString& FilePath, FSkeletalMeshRenderData& OutSkeletonMeshRenderData)
{
}

bool FFbxManager::LoadSkeletalMeshFromBinary(const FWString& FilePath, FSkeletalMeshRenderData& OutSkeletonMeshRenderData)
{
    
}
