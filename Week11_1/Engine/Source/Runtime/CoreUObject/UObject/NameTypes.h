#pragma once
#include "Core/HAL/PlatformType.h"
#include <functional>

class FArchive;
class FString;

enum :uint16 { NAME_SIZE = 256 };

enum ENameNone : uint8 { NAME_None = 0 };

class FName
{
    friend struct FNameHelper;

    uint32 DisplayIndex;    // 원본 문자열의 Hash
    uint32 ComparisonIndex; // 비교시 사용되는 Hash

public:
    FName() : DisplayIndex(0), ComparisonIndex(0) {}
    FName(ENameNone) : DisplayIndex(NAME_None), ComparisonIndex(NAME_None) {}
    FName(const WIDECHAR* Name);
    FName(const ANSICHAR* Name);
    FName(const FString& Name);

    FString ToString() const;
    uint32 GetDisplayIndex() const { return DisplayIndex; }
    uint32 GetComparisonIndex() const { return ComparisonIndex; }

    bool operator==(const FName& Other) const;

    void Serialize(FArchive& Ar) const;
    void Deserialize(FArchive& Ar);
};

template<>
struct std::hash<FName>
{
    size_t operator()(const FName& Name) const
    {
        return std::hash<uint32>{}(Name.GetDisplayIndex()) ^ (std::hash<uint32>{}(Name.GetComparisonIndex()) << 1);
    }
};