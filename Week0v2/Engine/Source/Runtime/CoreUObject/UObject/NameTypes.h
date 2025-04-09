#pragma once
#include "Core/HAL/PlatformType.h"
#include <functional>

class FString;


class FName
{
    friend struct FNameHelper;

    uint32 DisplayIndex;    // 원본 문자열의 Hash
    uint32 ComparisonIndex; // 비교시 사용되는 Hash

public:
    FName() : DisplayIndex(0), ComparisonIndex(0) {}
    FName(const WIDECHAR* Name);
    FName(const ANSICHAR* Name);
    FName(const FString& Name);

    FString ToString() const;
    uint32 GetDisplayIndex() const { return DisplayIndex; }
    uint32 GetComparisonIndex() const { return ComparisonIndex; }

    bool operator==(const FName& Other) const;
};

template<>
struct std::hash<FName>
{
    size_t operator()(const FName& Name) const
    {
        return std::hash<uint32>{}(Name.GetDisplayIndex()) ^ (std::hash<uint32>{}(Name.GetComparisonIndex()) << 1);
    }
};