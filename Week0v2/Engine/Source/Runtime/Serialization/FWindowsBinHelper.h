#pragma once
#include "Serialization/Archive.h"
#include "Core/Container/String.h"
#include "Engine/AssetDefine.h"
#include <filesystem>

class FWindowsBinHelper
{
public:
	// 직렬화 바이너리 데이터가 저장되어 있는 Archive를 Binary로 저장합니다.
	static void SaveToBin(const FString& SaveFilePath, FArchive& Archive);

	// 경로에서 직렬화 바이너리 데이터를 로드하여 FArchive로 반환합니다.
	static bool LoadFromBin(const FString& LoadFilePath, FArchive& OutArchive);
};

inline void FWindowsBinHelper::SaveToBin(const FString& SaveFilePath, FArchive& Archive)
{
	std::string pathString = *SaveFilePath;

	std::filesystem::path full_path(pathString);

	std::filesystem::create_directories(full_path.parent_path());

	Archive.SaveToFile(full_path.string());
}

inline bool FWindowsBinHelper::LoadFromBin(const FString& LoadFilePath, FArchive& OutArchive)
{
	OutArchive.LoadFromFile(*LoadFilePath);
	return !OutArchive.IsEmpty();
}