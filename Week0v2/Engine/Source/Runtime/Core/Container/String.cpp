#include "String.h"
#include <algorithm>
#include <cctype>

#include "Math/MathUtility.h"


#if USE_WIDECHAR
std::wstring FString::ConvertToWideChar(const ANSICHAR* NarrowStr)
{
	const int Size = MultiByteToWideChar(CP_UTF8, 0, NarrowStr, -1, nullptr, 0);
	std::wstring Str;
	Str.resize(Size - 1);
	MultiByteToWideChar(CP_UTF8, 0, NarrowStr, -1, Str.data(), Size);
	return Str;
}
#endif


FString FString::SanitizeFloat(float InFloat)
{
#if USE_WIDECHAR
    return FString{std::to_wstring(InFloat)};
#else
    return FString{std::to_string(InFloat)};
#endif
}

float FString::ToFloat(const FString& InString)
{
	return std::stof(*InString);
}

void FString::Empty()
{
    PrivateString.clear();
}

bool FString::Equals(const FString& Other, ESearchCase::Type SearchCase) const
{
    const int32 Num = Len();
    const int32 OtherNum = Other.Len();

    if (Num != OtherNum)
    {
        // Handle special case where FString() == FString("")
        return Num + OtherNum == 1;
    }
    else if (Num > 1)
    {
        if (SearchCase == ESearchCase::CaseSensitive)
        {
        	return TCString<ElementType>::Strcmp(**this, *Other) == 0;
        }
        else
        {
        	return std::ranges::equal(
		        PrivateString, Other.PrivateString, [](char a, char b)
	        {
		        return std::tolower(a) == std::tolower(b);
	        });
        }
    }

    return true;
}

bool FString::Contains(const FString& SubStr, ESearchCase::Type SearchCase, ESearchDir::Type SearchDir) const
{
    return Find(SubStr, SearchCase, SearchDir, 0) != INDEX_NONE;
}

int32 FString::Find(
    const FString& SubStr, ESearchCase::Type SearchCase, ESearchDir::Type SearchDir, int32 StartPosition
) const
{
    if (SubStr.IsEmpty() || IsEmpty())
    {
        return INDEX_NONE;
    }

    const ElementType* StrPtr = **this;
    const ElementType* SubStrPtr = *SubStr;
    const int32 StrLen = Len();
    const int32 SubStrLen = SubStr.Len();

    auto CompareFunc = [SearchCase](ElementType A, ElementType B) -> bool {
        return (SearchCase == ESearchCase::IgnoreCase) ? 
            tolower(A) == tolower(B) : A == B;
    };

    auto FindSubString = [&](int32 Start, int32 End, int32 Step) -> int32 {
        for (int32 i = Start; i != End; i += Step)
        {
            bool Found = true;
            for (int32 j = 0; j < SubStrLen; ++j)
            {
                if (!CompareFunc(StrPtr[i + j], SubStrPtr[j]))
                {
                    Found = false;
                    break;
                }
            }
            if (Found)
            {
                return i;
            }
        }
        return INDEX_NONE;
    };

    if (SearchDir == ESearchDir::FromStart)
    {
        StartPosition = FMath::Clamp(StartPosition, 0, StrLen - SubStrLen);
        return FindSubString(StartPosition, StrLen - SubStrLen + 1, 1);
    }
    else // ESearchDir::FromEnd
    {
        StartPosition = (StartPosition == INDEX_NONE) ? StrLen - SubStrLen : FMath::Min(StartPosition, StrLen - SubStrLen);
        return FindSubString(StartPosition, -1, -1);
    }
}

FString FString::Printf(const ElementType* Format, ...)
{
    if (!Format) // 포맷 문자열 null 체크
    {
        return FString();
    }

    // 첫 번째 시도: 스택에 작은 버퍼를 할당 (일반적인 경우를 빠르게 처리)
    ElementType StaticBuffer[512];
    va_list ArgPtr;
    va_start(ArgPtr, Format);

    int32 Result = -1;
#if USE_WIDECHAR
    #ifdef _WIN32
        // _vsnwprintf는 널 종료를 보장하지 않을 수 있으며, 성공 시 문자 수(널 제외) 또는 버퍼가 작으면 -1 반환
        Result = _vsnwprintf(StaticBuffer, sizeof(StaticBuffer) / sizeof(ElementType), Format, ArgPtr);
    #else
        // vswprintf는 C99 표준부터 버퍼 크기를 받고 널 종료를 보장. 성공 시 문자 수(널 제외), 오류 시 음수 반환.
        Result = vswprintf(StaticBuffer, sizeof(StaticBuffer) / sizeof(ElementType), Format, ArgPtr);
    #endif
#else
    // vsnprintf는 C99 표준부터 버퍼 크기를 받고 널 종료를 보장. 성공 시 문자 수(널 제외), 오류 시 음수 반환.
    Result = vsnprintf(StaticBuffer, sizeof(StaticBuffer) / sizeof(ElementType), Format, ArgPtr);
#endif
    va_end(ArgPtr);

    // 작은 버퍼로 충분했고 오류가 없었다면 바로 반환
    if (Result >= 0 && Result < static_cast<int32>(sizeof(StaticBuffer) / sizeof(ElementType)))
    {
        // StaticBuffer[Result] = 0; // _vsnwprintf는 널 종료 보장 안할 수 있으나, vsnprintf/vswprintf C99는 함. 안전하게 추가 가능.
        return FString(BaseStringType(StaticBuffer));
    }
    else // 버퍼가 너무 작거나 오류 발생
    {
        // 두 번째 시도: 필요한 크기를 계산하여 동적 할당
        int32 RequiredSize = -1;
        va_list ArgPtr2;
        va_start(ArgPtr2, Format);
#if USE_WIDECHAR
    #ifdef _WIN32
        // _vsnwprintf에 null 버퍼와 0 크기를 전달하면 필요한 크기(널 포함 안함) 반환
        RequiredSize = _vsnwprintf(nullptr, 0, Format, ArgPtr2);
    #else
        // C99 vswprintf 동작은 구현에 따라 다를 수 있음. 일반적으로 비슷하게 동작.
        RequiredSize = vswprintf(nullptr, 0, Format, ArgPtr2); // 이 방식이 표준은 아닐 수 있음. 대안 필요시 다른 라이브러리 사용.
        // 임시 버퍼를 크게 잡고 시도하는 방법도 있음.
        // 또는 C++20 std::format 사용 고려.
    #endif
#else
        // C99 vsnprintf에 null 버퍼와 0 크기를 전달하면 필요한 크기(널 포함 안함) 반환
        RequiredSize = vsnprintf(nullptr, 0, Format, ArgPtr2);
#endif
        va_end(ArgPtr2);

        if (RequiredSize < 0) // 크기 계산 실패 (오류)
        {
            // 오류 로그 출력 가능
            return FString(); // 빈 문자열 반환
        }

        // 필요한 크기 + 널 종료 문자 공간 할당
        std::vector<ElementType> DynamicBuffer(RequiredSize + 1);

        // 다시 포맷팅 수행
        va_list ArgPtr3;
        va_start(ArgPtr3, Format);
#if USE_WIDECHAR
    #ifdef _WIN32
        Result = _vsnwprintf(DynamicBuffer.data(), DynamicBuffer.size(), Format, ArgPtr3);
    #else
        Result = vswprintf(DynamicBuffer.data(), DynamicBuffer.size(), Format, ArgPtr3);
    #endif
#else
        Result = vsnprintf(DynamicBuffer.data(), DynamicBuffer.size(), Format, ArgPtr3);
#endif
        va_end(ArgPtr3);

        if (Result >= 0 && Result < static_cast<int32>(DynamicBuffer.size()))
        {
            // DynamicBuffer[Result] = 0; // 널 종료 보장됨 (C99)
            return FString(BaseStringType(DynamicBuffer.data()));
        }
        else
        {
            // 최종 포맷팅 실패 (이론상 발생하기 어려움)
            // 오류 로그 출력 가능
            return FString(); // 빈 문자열 반환
        }
    }
}

