#pragma once

// 필요한 표준 라이브러리 헤더 포함
#include <cctype>  // std::isdigit (char용)
#include <cwctype> // std::iswdigit (wchar_t용)
#include "HAL/PlatformType.h"

struct FChar
{
    /**
     * 주어진 문자가 10진수 숫자('0'부터 '9')인지 확인합니다.
     * @param Ch 확인할 TCHAR 문자.
     * @return 문자가 숫자이면 true, 그렇지 않으면 false를 반환합니다.
     */
    static inline bool IsDigit(TCHAR Ch)
    {
#if USE_WIDECHAR // TCHAR가 wchar_t인 경우
        // wint_t로 캐스팅하여 iswdigit 호출 (wchar_t는 안전하게 wint_t로 변환됨)
        return std::iswdigit(static_cast<wint_t>(Ch)) != 0;
#else // TCHAR가 char인 경우
        // unsigned char로 캐스팅하여 isdigit 호출 (음수 char 값 문제를 피하기 위함)
        // isdigit는 int를 받지만 unsigned char는 안전하게 int로 변환됨
        return std::isdigit(static_cast<unsigned char>(Ch)) != 0;
#endif
    }
};
