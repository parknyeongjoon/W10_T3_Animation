#pragma once
#include <cstring>
#include <cwchar>
#include <cctype>
#include <cstdlib> // String to number conversions (atoi, atof, strtol, strtod, etc.)
#include <cwctype>

#include "HAL/PlatformType.h"

template <typename T>
struct TCString
{
public:
    using CharType = T;

private:
    // 문자가 영숫자인지 확인
    static constexpr bool IsAlnum(CharType C)
    {
        if constexpr (std::is_same_v<CharType, char>)
        {
            return std::isalnum(static_cast<unsigned char>(C));
        }
        else if constexpr (std::is_same_v<CharType, wchar_t>)
        {
            return std::iswalnum(static_cast<wint_t>(C));
        }
        return false; // Unsupported type
    }

    // 문자를 대문자로 변환
    static constexpr CharType ToUpper(CharType C)
    {
        if constexpr (std::is_same_v<CharType, char>)
        {
            return static_cast<CharType>(std::toupper(static_cast<unsigned char>(C)));
        }
        else if constexpr (std::is_same_v<CharType, wchar_t>)
        {
            return static_cast<CharType>(std::towupper(static_cast<wint_t>(C)));
        }
        return C; // Unsupported type
    }

    // 문자가 공백인지 확인 (char/wchar_t 호환)
    static bool IsSpace(CharType C)
    {
        if constexpr (std::is_same_v<CharType, char>)
        {
            return std::isspace(static_cast<unsigned char>(C));
        }
        else if constexpr (std::is_same_v<CharType, wchar_t>)
        {
            return std::iswspace(static_cast<wint_t>(C));
        }
        return false;
    }

    // 문자를 소문자로 변환 (char/wchar_t 호환)
    static CharType ToLower(CharType C)
    {
        if constexpr (std::is_same_v<CharType, char>)
        {
            return static_cast<CharType>(std::tolower(static_cast<unsigned char>(C)));
        }
        else if constexpr (std::is_same_v<CharType, wchar_t>)
        {
            return static_cast<CharType>(std::towlower(static_cast<wint_t>(C)));
        }
        return C;
    }

public:
    // --- 문자열 길이 ---
    /** 문자열의 길이를 반환합니다 (널 종료 문자 제외). */
    static std::size_t Strlen(const CharType* str)
    {
        if (!str) return 0; // Null 체크 추가
        if constexpr (std::is_same_v<CharType, char>) { return std::strlen(str); }
        else if constexpr (std::is_same_v<CharType, wchar_t>) { return std::wcslen(str); }
        else
        {
            /* static_assert(false, "Unsupported character type!"); */
            return 0;
        }
    }

    // --- 문자열 복사 ---
    /** 문자열 src를 dest에 복사합니다 (널 종료 문자 포함). dest 버퍼는 충분히 커야 합니다. */
    static CharType* Strcpy(CharType* dest, const CharType* src)
    {
        if (!dest || !src) return dest; // Null 체크
        if constexpr (std::is_same_v<CharType, char>) { return std::strcpy(dest, src); }
        else if constexpr (std::is_same_v<CharType, wchar_t>) { return std::wcscpy(dest, src); }
        else
        {
            /* static_assert(false, "Unsupported character type!"); */
            return nullptr;
        }
    }

    template <typename CharType>
    static CharType* Strncpy(CharType* dest, const CharType* src, std::size_t count)
    {
        if (!dest || !src || count == 0) return dest; // Null/Zero 체크

        if constexpr (std::is_same_v<CharType, char>)
        {
            // strncpy_s는 반환값이 errno_t(0이 성공), dest를 반환하려면 직접 반환
            strncpy_s(dest, count, src, _TRUNCATE);
            return dest;
        }
        else if constexpr (std::is_same_v<CharType, wchar_t>)
        {
            wcsncpy_s(dest, count, src, _TRUNCATE);
            return dest;
        }
        else
        {
            static_assert(std::is_same_v<CharType, void>, "Unsupported character type!");
            return nullptr;
        }
    }

    // --- 문자열 합치기 ---
    /** 문자열 src를 dest 끝에 이어 붙입니다 (널 종료 문자 포함). dest 버퍼는 충분히 커야 합니다. */
    static CharType* Strcat(CharType* dest, const CharType* src)
    {
        if (!dest || !src) return dest; // Null 체크
        if constexpr (std::is_same_v<CharType, char>) { return std::strcat(dest, src); }
        else if constexpr (std::is_same_v<CharType, wchar_t>) { return std::wcscat(dest, src); }
        else
        {
            /* static_assert(false, "Unsupported character type!"); */
            return nullptr;
        }
    }

    // --- 문자열 비교 ---
    /** 두 문자열 str1과 str2를 비교합니다. */
    static int Strcmp(const CharType* str1, const CharType* str2)
    {
        if (!str1 || !str2) return (str1 == str2) ? 0 : (str1 ? 1 : -1); // Null 처리
        if constexpr (std::is_same_v<CharType, char>) { return std::strcmp(str1, str2); }
        else if constexpr (std::is_same_v<CharType, wchar_t>) { return std::wcscmp(str1, str2); }
        else
        {
            /* static_assert(false, "Unsupported character type!"); */
            return 0;
        }
    }

    /** 두 문자열 str1과 str2의 첫 count개의 문자를 비교합니다. */
    static int Strncmp(const CharType* str1, const CharType* str2, std::size_t count)
    {
        if (count == 0) return 0;
        if (!str1 || !str2) return (str1 == str2) ? 0 : (str1 ? 1 : -1); // Null 처리
        if constexpr (std::is_same_v<CharType, char>) { return std::strncmp(str1, str2, count); }
        else if constexpr (std::is_same_v<CharType, wchar_t>) { return std::wcsncmp(str1, str2, count); }
        else
        {
            /* static_assert(false, "Unsupported character type!"); */
            return 0;
        }
    }

    /** 두 문자열 str1과 str2를 대소문자 구분 없이 비교합니다. (char 버전) */
    static int Stricmp(const char* str1, const char* str2)
    {
        if (!str1 || !str2) return (str1 == str2) ? 0 : (str1 ? 1 : -1);
        while (*str1 && (std::tolower(static_cast<unsigned char>(*str1)) == std::tolower(static_cast<unsigned char>(*str2))))
        {
            ++str1;
            ++str2;
        }
        return std::tolower(static_cast<unsigned char>(*str1)) - std::tolower(static_cast<unsigned char>(*str2));
    }

    /** 두 문자열 str1과 str2를 대소문자 구분 없이 비교합니다. (wchar_t 버전) */
    static int Stricmp(const wchar_t* str1, const wchar_t* str2)
    {
        if (!str1 || !str2) return (str1 == str2) ? 0 : (str1 ? 1 : -1);
        while (*str1 && (std::towlower(static_cast<wint_t>(*str1)) == std::towlower(static_cast<wint_t>(*str2))))
        {
            ++str1;
            ++str2;
        }
        return static_cast<int>(std::towlower(static_cast<wint_t>(*str1))) - static_cast<int>(std::towlower(static_cast<wint_t>(*str2)));
    }

    /** 두 문자열 str1과 str2의 첫 count개의 문자를 대소문자 구분 없이 비교합니다. (char 버전) */
    static int Strnicmp(const char* str1, const char* str2, std::size_t count)
    {
        if (count == 0) return 0;
        if (!str1 || !str2) return (str1 == str2) ? 0 : (str1 ? 1 : -1);
        while (count-- > 0 && *str1 && (std::tolower(static_cast<unsigned char>(*str1)) == std::tolower(static_cast<unsigned char>(*str2))))
        {
            if (*str1 == '\0' || count == 0) break;
            ++str1;
            ++str2;
        }
        return count ? (std::tolower(static_cast<unsigned char>(*str1)) - std::tolower(static_cast<unsigned char>(*str2))) : 0;
    }

    /** 두 문자열 str1과 str2의 첫 count개의 문자를 대소문자 구분 없이 비교합니다. (wchar_t 버전) */
    static int Strnicmp(const wchar_t* str1, const wchar_t* str2, std::size_t count)
    {
        if (count == 0) return 0;
        if (!str1 || !str2) return (str1 == str2) ? 0 : (str1 ? 1 : -1);
        while (count-- > 0 && *str1 && (std::towlower(static_cast<wint_t>(*str1)) == std::towlower(static_cast<wint_t>(*str2))))
        {
            if (*str1 == L'\0' || count == 0) break;
            ++str1;
            ++str2;
        }
        return count
                   ? (static_cast<int>(std::towlower(static_cast<wint_t>(*str1))) - static_cast<int>(std::towlower(static_cast<wint_t>(*str2))))
                   : 0;
    }

    // --- 문자 검색 ---
    /** 문자열 str에서 문자 ch가 처음 나타나는 위치를 찾습니다. */
    static const CharType* Strchr(const CharType* str, int ch) // 표준 함수는 int를 받음
    {
        if (!str) return nullptr;
        CharType c = static_cast<CharType>(ch); // int를 CharType으로 캐스팅
        if constexpr (std::is_same_v<CharType, char>) { return std::strchr(str, c); }
        else if constexpr (std::is_same_v<CharType, wchar_t>) { return std::wcschr(str, c); }
        else
        {
            /* static_assert(false, "Unsupported character type!"); */
            return nullptr;
        }
    }

    /** 쓰기 가능한 포인터 반환 버전 */
    static CharType* Strchr(CharType* str, int ch)
    {
        return const_cast<CharType*>(Strchr(const_cast<const CharType*>(str), ch));
    }

    /** 문자열 str에서 문자 ch가 마지막으로 나타나는 위치를 찾습니다. */
    static const CharType* Strrchr(const CharType* str, int ch)
    {
        if (!str) return nullptr;
        CharType c = static_cast<CharType>(ch);
        if constexpr (std::is_same_v<CharType, char>) { return std::strrchr(str, c); }
        else if constexpr (std::is_same_v<CharType, wchar_t>) { return std::wcsrchr(str, c); }
        else
        {
            /* static_assert(false, "Unsupported character type!"); */
            return nullptr;
        }
    }

    /** 쓰기 가능한 포인터 반환 버전 */
    static CharType* Strrchr(CharType* str, int ch)
    {
        return const_cast<CharType*>(Strrchr(const_cast<const CharType*>(str), ch));
    }

    // --- 부분 문자열 검색 ---
    /** 문자열 str1 내에서 문자열 str2가 처음 나타나는 위치를 찾습니다. */
    static const CharType* Strstr(const CharType* str1, const CharType* str2)
    {
        if (!str1 || !str2) return nullptr; // 하나라도 null이면 찾을 수 없음
        if (!*str2) return str1;            // 찾는 문자열이 비어있으면 시작 위치 반환
        if constexpr (std::is_same_v<CharType, char>) { return std::strstr(str1, str2); }
        else if constexpr (std::is_same_v<CharType, wchar_t>) { return std::wcsstr(str1, str2); }
        else
        {
            /* static_assert(false, "Unsupported character type!"); */
            return nullptr;
        }
    }

    /** 쓰기 가능한 포인터 반환 버전 */
    static CharType* Strstr(CharType* str1, const CharType* str2)
    {
        return const_cast<CharType*>(Strstr(const_cast<const CharType*>(str1), str2));
    }


    /**
     * 문자열 Str 내에서 문자열 Find를 대소문자 구분 없이 찾습니다.
     * 단, 찾은 위치 바로 앞의 문자가 영숫자가 아니어야 합니다.
     * @param Str 검색 대상 문자열.
     * @param Find 찾을 문자열.
     * @param bSkipQuotedChars true이면 큰따옴표(") 안의 내용은 검색에서 제외합니다.
     * @return 찾은 위치의 포인터, 찾지 못하면 nullptr.
     */
    static const CharType* Strifind(const CharType* Str, const CharType* Find, bool bSkipQuotedChars = false)
    {
        if (Find == nullptr || Str == nullptr)
        {
            return nullptr;
        }

        // 찾는 문자열이 비어있으면 시작 위치 반환 (표준 strstr 동작과 일치)
        if (*Find == TEXT('\0'))
        {
            return Str;
        }

        // 첫 문자(대문자)와 나머지 길이 계산
        CharType FindInitialUpper = ToUpper(*Find);
        int32 Length = static_cast<int32>(Strlen(Find)) - 1; // 첫 문자 제외 길이
        if (Length < 0) Length = 0;                          // 혹시 Find가 한 글자일 경우 대비

        const CharType* FindRest = Find + 1; // Find의 두 번째 문자부터 포인터

        bool bAlnumPrev = false; // 이전 문자가 영숫자였는지 여부 (시작은 true로 가정하여 첫 문자 바로 앞은 조건 만족 못하게 함)
        CharType CurrentChar = *Str;

        if (bSkipQuotedChars)
        {
            bool bInQuotedStr = false;
            while (CurrentChar != TEXT('\0'))
            {
                CharType CurrentUpper = ToUpper(CurrentChar);

                // 따옴표 밖에 있고, 이전 문자가 영숫자가 아니며, 현재 문자가 Find의 첫 문자와 같고, 나머지가 일치하는지 확인
                if (!bInQuotedStr && !bAlnumPrev && CurrentUpper == FindInitialUpper && Strnicmp(Str + 1, FindRest, Length) == 0)
                {
                    return Str; // 찾았으면 현재 위치 반환
                }

                // 다음 반복을 위해 현재 문자가 영숫자인지 기록
                bAlnumPrev = IsAlnum(CurrentChar);

                // 따옴표 상태 토글
                if (CurrentChar == TEXT('"'))
                {
                    bInQuotedStr = !bInQuotedStr;
                }

                // 다음 문자로 이동
                Str++;
                CurrentChar = *Str;
            }
        }
        else // 따옴표 무시 로직
        {
            while (CurrentChar != TEXT('\0'))
            {
                CharType CurrentUpper = ToUpper(CurrentChar);

                // 이전 문자가 영숫자가 아니며, 현재 문자가 Find의 첫 문자와 같고, 나머지가 일치하는지 확인
                if (!bAlnumPrev && CurrentUpper == FindInitialUpper && Strnicmp(Str + 1, FindRest, Length) == 0)
                {
                    return Str; // 찾았으면 현재 위치 반환
                }

                // 다음 반복을 위해 현재 문자가 영숫자인지 기록
                bAlnumPrev = IsAlnum(CurrentChar);

                // 다음 문자로 이동
                Str++;
                CurrentChar = *Str;
            }
        }

        return nullptr; // 끝까지 못 찾음
    }

    /** Strifind의 비-const 오버로드 */
    static CharType* Strifind(CharType* Str, const CharType* Find, bool bSkipQuotedChars = false)
    {
        return const_cast<CharType*>(Strifind(const_cast<const CharType*>(Str), Find, bSkipQuotedChars));
    }

    // --- 문자열 범위 함수 ---
    /** str1 시작 부분에서 str2에 포함된 문자들로만 구성된 초기 세그먼트 길이 */
    static std::size_t Strspn(const CharType* str1, const CharType* str2)
    {
        if (!str1 || !str2) return 0;
        if constexpr (std::is_same_v<CharType, char>) { return std::strspn(str1, str2); }
        else if constexpr (std::is_same_v<CharType, wchar_t>) { return std::wcsspn(str1, str2); }
        else
        {
            /* static_assert(false, "Unsupported character type!"); */
            return 0;
        }
    }

    /** str1에서 str2에 포함된 문자가 처음 나타나는 위치까지의 초기 세그먼트 길이 */
    static std::size_t Strcspn(const CharType* str1, const CharType* str2)
    {
        if (!str1 || !str2) return Strlen(str1); // str2가 null이면 str1 전체 길이
        if constexpr (std::is_same_v<CharType, char>) { return std::strcspn(str1, str2); }
        else if constexpr (std::is_same_v<CharType, wchar_t>) { return std::wcscspn(str1, str2); }
        else
        {
            /* static_assert(false, "Unsupported character type!"); */
            return 0;
        }
    }

    // --- 대소문자 변환 ---
    /** 문자열 전체를 대문자로 변환합니다. */
    static CharType* Strupr(CharType* str)
    {
        if (!str) return nullptr;
        CharType* p = str;
        while (*p)
        {
            if constexpr (std::is_same_v<CharType, char>) { *p = static_cast<CharType>(std::toupper(static_cast<unsigned char>(*p))); }
            else if constexpr (std::is_same_v<CharType, wchar_t>) { *p = static_cast<CharType>(std::towupper(static_cast<wint_t>(*p))); }
            ++p;
        }
        return str;
    }

    /** 문자열 전체를 소문자로 변환합니다. */
    static CharType* Strlwr(CharType* str)
    {
        if (!str) return nullptr;
        CharType* p = str;
        while (*p)
        {
            if constexpr (std::is_same_v<CharType, char>) { *p = static_cast<CharType>(std::tolower(static_cast<unsigned char>(*p))); }
            else if constexpr (std::is_same_v<CharType, wchar_t>) { *p = static_cast<CharType>(std::towlower(static_cast<wint_t>(*p))); }
            ++p;
        }
        return str;
    }

    // --- 숫자 변환 ---
    /** 문자열을 정수(int)로 변환합니다. */
    static int Atoi(const CharType* str)
    {
        if (!str) return 0;
        if constexpr (std::is_same_v<CharType, char>) { return std::atoi(str); }
        else if constexpr (std::is_same_v<CharType, wchar_t>) { return static_cast<int>(std::wcstol(str, nullptr, 10)); } // wcstol 사용 및 캐스팅
        else
        {
            /* static_assert(false, "Unsupported character type!"); */
            return 0;
        }
    }

    /** 문자열을 long long 정수로 변환합니다. */
    static long long Atoll(const CharType* str)
    {
        if (!str) return 0;
        if constexpr (std::is_same_v<CharType, char>) { return std::atoll(str); }
        else if constexpr (std::is_same_v<CharType, wchar_t>) { return std::wcstoll(str, nullptr, 10); }
        else
        {
            /* static_assert(false, "Unsupported character type!"); */
            return 0;
        }
    }

    /** 문자열을 부동 소수점 수(double)로 변환합니다. */
    static double Atod(const CharType* str)
    {
        if (!str) return 0.0;
        if constexpr (std::is_same_v<CharType, char>) { return std::atof(str); } // atof는 double 반환
        else if constexpr (std::is_same_v<CharType, wchar_t>) { return std::wcstod(str, nullptr); }
        else
        {
            /* static_assert(false, "Unsupported character type!"); */
            return 0.0;
        }
    }

    /** 문자열을 부동 소수점 수(float)로 변환합니다. */
    static float Atof(const CharType* str)
    {
        // Atod를 호출하고 float으로 캐스팅
        return static_cast<float>(Atod(str));
    }

    /**
     * 문자열 nptr의 초기 부분을 base 진법에 따라 long int 값으로 변환합니다.
     * @param nptr 변환할 문자열 포인터.
     * @param endptr [출력] 변환에 사용된 마지막 문자 다음 문자를 가리키는 포인터 주소. nullptr일 수 있음.
     * @param base 변환에 사용할 진법 (0, 2-36). 0이면 접두사("0x" 또는 "0")로 자동 감지.
     * @return 변환된 long int 값. 변환 불가 시 0 반환. 범위를 벗어나면 LONG_MAX 또는 LONG_MIN 반환 (errno 설정됨).
     */
    static long int Strtoi(const CharType* nptr, CharType** endptr, int base)
    {
        // 입력 포인터 null 체크
        if (!nptr)
        {
            if (endptr) *endptr = const_cast<CharType*>(nptr); // endptr도 null로 설정
            return 0;
        }

        // 타입에 따라 표준 함수 호출
        if constexpr (std::is_same_v<CharType, char>)
        {
            // errno = 0; // 필요시 오버플로우/언더플로우 체크를 위해 errno 초기화
            return std::strtol(nptr, endptr, base);
        }
        else if constexpr (std::is_same_v<CharType, wchar_t>)
        {
            // errno = 0;
            return std::wcstol(nptr, endptr, base);
        }
        else
        {
            // static_assert(false, "Unsupported character type for Strtoi!");
            if (endptr) *endptr = const_cast<CharType*>(nptr); // 변환 안 됐으므로 시작 위치 가리킴
            return 0;                                          // 지원되지 않는 타입
        }
    }

    // --- 새로 추가된 ToBool 함수 ---
    /**
     * Converts a string into a boolean value
     *   1, "True", "Yes" and non-zero integers become true
     *   0, "False", "No" and unparsable values become false
     *
     * @param str 변환할 문자열 포인터.
     * @return The boolean value
     */
    static bool ToBool(const CharType* str)
    {
        // 1. Null 또는 빈 문자열 확인 -> false
        if (!str || *str == TEXT('\0'))
        {
            return false;
        }

        // 2. "True" 또는 "Yes" (대소문자 무시) 확인 -> true
        if (Stricmp(str, TEXT("True")) == 0 ||
            Stricmp(str, TEXT("Yes")) == 0)
        {
            return true;
        }

        // 3. "False" 또는 "No" (대소문자 무시) 확인 -> false
        if (Stricmp(str, TEXT("False")) == 0 ||
            Stricmp(str, TEXT("No")) == 0)
        {
            return false;
        }

        // 4. 정수로 파싱 시도
        CharType* endptr = nullptr;
        long int intValue = Strtoi(str, &endptr, 0); // 자동 진법 감지 (10진수, 8진수, 16진수)

        // 파싱이 성공했는지 확인
        if (endptr != str) // 숫자를 하나라도 읽었다면
        {
            // 숫자 뒤에 남은 문자가 공백 뿐인지 확인
            while (IsSpace(*endptr))
            {
                endptr++;
            }

            // 숫자와 후행 공백 이후 문자열 끝에 도달했다면 유효한 정수 문자열로 간주
            if (*endptr == TEXT('\0'))
            {
                // 정수 값이 0이 아니면 true, 0이면 false 반환
                return (intValue != 0);
            }
        }

        // 5. 위의 어떤 조건에도 해당하지 않으면 (파싱 불가 등) -> false
        return false;
    }


    // TODO: 필요에 따라 strtol, strtod, strtoull 등 더 상세한 숫자 변환 함수 래퍼 추가 가능
};

using FCString = TCString<TCHAR>;
using FCStringAnsi = TCString<ANSICHAR>;
using FCStringWide = TCString<WIDECHAR>;
