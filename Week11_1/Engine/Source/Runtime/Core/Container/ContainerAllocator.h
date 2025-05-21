#pragma once
#include <iostream>

#include "Core/HAL/PlatformType.h"
#include "Core/HAL/PlatformMemory.h"


/**
 * IndexSize에 따라 적절한 정수 타입을 매핑하는 메타 프로그래밍 구조체입니다.
 * 
 * @tparam IndexSize 지원되는 인덱스 크기 (8, 16, 32, 64 중 하나)
 * 
 * TBitsToSizeType은 템플릿 인자로 전달된 IndexSize 값에 따라
 * 해당 크기에 맞는 정수 타입을 Type으로 정의합니다.
 * 
 * - IndexSize = 8  -> Type = int8
 * - IndexSize = 16 -> Type = int16
 * - IndexSize = 32 -> Type = int32
 * - IndexSize = 64 -> Type = int64
 * 
 * 지원되지 않는 IndexSize를 전달하면 컴파일 타임에 static_assert를 통해 에러를 발생시킵니다.
 */
template <int IndexSize>
struct TBitsToSizeType
{
    // ReSharper disable once CppStaticAssertFailure
    // IndexSize가 8, 16, 32, 64중에 하나가 아니라면 컴파일 에러
    //static_assert(false, "Unsupported allocator index size.");
};

template <> struct TBitsToSizeType<8>  { using Type = int8; };
template <> struct TBitsToSizeType<16> { using Type = int16; };
template <> struct TBitsToSizeType<32> { using Type = int32; };
template <> struct TBitsToSizeType<64> { using Type = int64; };


/**
 * Container에 사용되는 Allocator
 * @tparam T 컨테이너 타입
 * @tparam IndexSize 최대 Index의 크기 (bit)
 */
template <typename T, int IndexSize>
struct TContainerAllocator
{
public:
    using SizeType = typename TBitsToSizeType<IndexSize>::Type;

    //~ std::allocator_traits 관련 타입
    using value_type = T;
    using size_type = std::make_unsigned_t<SizeType>;
    using difference_type = std::make_signed_t<SizeType>;
    using propagate_on_container_move_assignment = std::true_type;
    using is_always_equal = std::true_type;

    template <typename U>
    struct rebind
    {
        using other = TContainerAllocator<U, IndexSize>;
    };
    //~ std::allocator_traits 관련 타입

public:

    constexpr TContainerAllocator() noexcept = default;

    constexpr TContainerAllocator(const TContainerAllocator&) noexcept = default;
    constexpr TContainerAllocator& operator=(const TContainerAllocator&) = default;
    constexpr TContainerAllocator(TContainerAllocator&&) noexcept = default;
    constexpr TContainerAllocator& operator=(TContainerAllocator&&) noexcept = default;

    template <class U>
    constexpr TContainerAllocator(const TContainerAllocator<U, IndexSize>&) noexcept {}

    constexpr ~TContainerAllocator() = default;

public:
    constexpr T* allocate(size_type n) noexcept;
    constexpr void deallocate(T* p, size_type n) noexcept;
};

template <typename T, int IndexSize>
constexpr T* TContainerAllocator<T, IndexSize>::allocate(size_type n) noexcept
{
    const size_t AllocSize = sizeof(T) * n;
    return static_cast<T*>(FPlatformMemory::Malloc<EAT_Container>(AllocSize));
}

template <typename T, int IndexSize>
constexpr void TContainerAllocator<T, IndexSize>::deallocate(T* p, size_type n) noexcept
{
    const size_t AllocSize = sizeof(T) * n;
    FPlatformMemory::Free<EAT_Container>(p, AllocSize);
}

template <typename T> using FDefaultAllocator = TContainerAllocator<T, 32>;
template <typename T> using FDefaultAllocator64 = TContainerAllocator<T, 64>;
