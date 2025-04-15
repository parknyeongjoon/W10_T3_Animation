#pragma once
#include <cstdint>

//~ Windows.h
#define _TCHAR_DEFINED  // TCHAR 재정의 에러 때문
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#undef max
#undef min

#ifdef TEXT             // Windows.h의 TEXT를 삭제
    #undef TEXT
#endif
//~ Windows.h


// inline을 강제하는 매크로
#define FORCEINLINE __forceinline

// inline을 하지않는 매크로
#define FORCENOINLINE __declspec(noinline)


#define USE_WIDECHAR 0

#if USE_WIDECHAR 
    #define TEXT(x) L##x
#else
    #define TEXT(x) x
#endif


// unsigned int type
typedef std::uint8_t uint8;
typedef std::uint16_t uint16;
typedef std::uint32_t uint32;
typedef std::uint64_t uint64;

// signed int
typedef std::int8_t int8;
typedef std::int16_t int16;
typedef std::int32_t int32;
typedef std::int64_t int64;

typedef char ANSICHAR;
typedef wchar_t WIDECHAR;

#if USE_WIDECHAR
typedef WIDECHAR TCHAR;
#else
typedef ANSICHAR TCHAR;
#endif

#if USE_WIDECHAR
inline WCHAR* ConvertAnsiToWchar(const ANSICHAR* ansiStr)
{
    if (ansiStr == nullptr)
    {
        return nullptr;
    }
    
    // ANSI 문자열을 변환할 때 CP_ACP (시스템 기본 ANSI 코드 페이지)를 사용합니다.
    // 필요한 버퍼 길이(널 종료 문자 포함)를 계산합니다.
    int bufferSize = MultiByteToWideChar(CP_ACP, 0, ansiStr, -1, nullptr, 0);
    if (bufferSize == 0)
    {
        // 변환 중 오류 발생
        return nullptr;
    }
    
    // WCHAR* 버퍼 동적 할당
    WCHAR* wstr = new WCHAR[bufferSize];
    
    // ANSI 문자열을 WCHAR*로 변환
    int result = MultiByteToWideChar(CP_ACP, 0, ansiStr, -1, wstr, bufferSize);
    if (result == 0)
    {
        // 변환 실패 시, 할당한 메모리 해제
        delete[] wstr;
        return nullptr;
    }
    
    return wstr;
}
#else
inline ANSICHAR* ConvertWcharToAnsi(const WCHAR* wideStr)
{
    if (wideStr == nullptr)
    {
        return nullptr;
    }
    
    // 첫 번째 호출: 변환 후 필요한 버퍼 길이(널 종료 문자를 포함)를 계산합니다.
    int bufferSize = WideCharToMultiByte(CP_ACP, 0, wideStr, -1, nullptr, 0, nullptr, nullptr);
    if (bufferSize == 0)
    {
        // 변환 실패 시
        return nullptr;
    }
    
    // ANSICHAR* 버퍼 동적 할당
    ANSICHAR* ansiStr = new ANSICHAR[bufferSize];
    
    // 실제 변환 수행
    int result = WideCharToMultiByte(CP_ACP, 0, wideStr, -1, ansiStr, bufferSize, nullptr, nullptr);
    if (result == 0)
    {
        // 변환 실패 시, 할당한 메모리 해제 후 nullptr 반환
        delete[] ansiStr;
        return nullptr;
    }
    
    return ansiStr;
}

#endif

// 임시로 사용
#include <string>
using FWString = std::wstring;
