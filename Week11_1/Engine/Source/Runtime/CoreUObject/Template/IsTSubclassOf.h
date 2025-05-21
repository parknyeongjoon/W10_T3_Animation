#pragma once

template <typename T>
struct TSubclassOf;

template <typename T>
static constexpr bool TIsTSubclassOf_V = false;

template <typename T> constexpr bool TIsTSubclassOf_V<               TSubclassOf<T>> = true;
template <typename T> constexpr bool TIsTSubclassOf_V<const          TSubclassOf<T>> = true;
template <typename T> constexpr bool TIsTSubclassOf_V<      volatile TSubclassOf<T>> = true;
template <typename T> constexpr bool TIsTSubclassOf_V<const volatile TSubclassOf<T>> = true;

template <typename T>
concept TIsTSubclassOf = TIsTSubclassOf_V<T>;
