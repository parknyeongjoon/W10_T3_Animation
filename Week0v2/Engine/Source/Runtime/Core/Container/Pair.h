#pragma once
#include <type_traits>
#include <utility>
#include "String.h"

template<typename FirstType, typename SecondType>
struct TPair
{
    FirstType Key;
    SecondType Value;

    // 기본 생성자
    constexpr TPair() : Key(), Value() {}

    // 값으로 초기화하는 생성자
    constexpr TPair(const FirstType& InFirst, const SecondType& InSecond)
        : Key(InFirst), Value(InSecond) {}

    // 이동 생성자
    constexpr TPair(FirstType&& InFirst, SecondType&& InSecond)
        : Key(std::move(InFirst)), Value(std::move(InSecond)) {}

    // 복사 생성자
    constexpr TPair(const TPair& Other) = default;

    // 이동 생성자
    constexpr TPair(TPair&& Other) noexcept = default;

    // 복사 할당 연산자
    TPair& operator=(const TPair& Other) = default;

    // 이동 할당 연산자
    TPair& operator=(TPair&& Other) noexcept = default;

    // 비교 연산자
    constexpr bool operator==(const TPair& Other) const
    {
        return Key == Other.Key && Value == Other.Value;
    }

    constexpr bool operator!=(const TPair& Other) const
    {
        return !(*this == Other);
    }

    // std::pair로 변환
    constexpr operator std::pair<FirstType, SecondType>() const
    {
        return std::pair<FirstType, SecondType>(Key, Value);
    }

    // std::pair에서 변환
    constexpr TPair(const std::pair<FirstType, SecondType>& StdPair)
        : Key(StdPair.first), Value(StdPair.second) {}
};

// 편의를 위한 MakePair 함수
template<typename FirstType, typename SecondType>
constexpr TPair<std::decay_t<FirstType>, std::decay_t<SecondType>> MakePair(FirstType&& InFirst, SecondType&& InSecond)
{
    return TPair<std::decay_t<FirstType>, std::decay_t<SecondType>>(
        std::forward<FirstType>(InFirst),
        std::forward<SecondType>(InSecond)
    );
}
