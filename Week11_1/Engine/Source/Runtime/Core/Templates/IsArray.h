#pragma once
#include <type_traits>


template <typename T>
concept TIsArray = std::is_array_v<T>;
