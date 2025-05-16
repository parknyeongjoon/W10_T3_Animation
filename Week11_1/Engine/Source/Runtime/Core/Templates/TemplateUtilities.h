// ReSharper disable CppClangTidyCppcoreguidelinesMissingStdForward
// ReSharper disable CppCStyleCast
// ReSharper disable CppClangTidyCppcoreguidelinesRvalueReferenceParamNotMoved
#pragma once
#include "HAL/PlatformType.h"


/**
 * 어떤 타입에 대해서도 항상 false를 반환하는 변수 템플릿입니다.
 *
 * 이 변수 템플릿은 템플릿 메타프로그래밍에서 특정 분기에서만 컴파일 타임 에러를 발생시키고 싶을 때
 * static_assert와 함께 자주 사용됩니다.
 *
 * @tparam T 사용되는 타입 파라미터 (실제 값에는 영향을 주지 않습니다)
 *
 * @note 템플릿 코드에서 static_assert를 통해 컴파일 에러를 발생시키고 싶을 때 유용합니다.
 * @see https://en.cppreference.com/w/cpp/language/variable_template
 */
template <typename T>
constexpr bool TAlwaysFalse = false;

/**
 * 객체를 우측값으로 캐스팅하여 이동 연산을 수행합니다.
 * rvalue나 const 객체에 대해서는 사용할 수 없도록 제한됩니다.
 * @param Obj 이동시킬 객체
 * @return 이동된 객체의 우측값 참조
 */
template <typename T>
FORCEINLINE constexpr std::remove_reference_t<T>&& MoveTemp(T&& Obj) noexcept
{
    using CastType = std::remove_reference_t<T>;

    // rvalue나 const 객체가 전달되지 않았는지 확인합니다.
    // rvalue는 불필요하고, const 객체는 거의 확실히 실수일 가능성이 있습니다.
    static_assert(std::is_lvalue_reference_v<T>, "MoveTemp called on an rvalue");
    static_assert(!std::same_as<CastType&, const CastType&>, "MoveTemp called on a const object");

    return (CastType&&)Obj;
}

/**
 * 비상수 객체를 복사합니다.
 * @param Val 복사할 객체
 * @return 복사된 객체
 */
template <typename T>
FORCEINLINE T CopyTemp(T& Val)
{
    return const_cast<const T&>(Val);
}

/**
 * 상수 객체를 복사합니다.
 * @param Val 복사할 객체
 * @return 복사된 객체
 */
template <typename T>
FORCEINLINE T CopyTemp(const T& Val)
{
    return Val;
}

/**
 * 좌측값 참조를 완벽 전달합니다.
 * @param Obj 전달할 객체
 * @return 전달된 객체의 참조
 */
template <typename T>
FORCEINLINE constexpr T&& Forward(std::remove_reference_t<T>& Obj) noexcept
{
    return (T&&)Obj;
}

/**
 * 우측값 참조를 완벽 전달합니다.
 * @param Obj 전달할 객체
 * @return 전달된 객체의 참조
 */
template <typename T>
FORCEINLINE constexpr T&& Forward(std::remove_reference_t<T>&& Obj) noexcept
{
    return (T&&)Obj;
}

/**
 * 두 객체의 값을 교환합니다.
 * @param A 첫 번째 객체
 * @param B 두 번째 객체
 */
template <typename T>
inline void Swap(T& A, T& B)
{
    T Temp = MoveTemp(A);
    A = MoveTemp(B);
    B = MoveTemp(Temp);
}
