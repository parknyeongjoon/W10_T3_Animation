#pragma once
#include "ObjectUtils.h"
#include "HAL/PlatformType.h"

/**
 * Object가 유효할때만 값을 반환하는 포인터, Object가 유효하지 않다면 nullptr
 * @tparam T UObject를 상속받은 Class
 *
 * @note TWeakObjectPtr의 소멸자가 호출이 되어도 Object는 따로 삭제를 하지 않습니다.
 */
template <typename T>
struct TWeakObjectPtr
{
public:
    using ElementType = T;

    TWeakObjectPtr() = default;
    ~TWeakObjectPtr() = default;

    TWeakObjectPtr(const TWeakObjectPtr&) = default;
    TWeakObjectPtr& operator=(const TWeakObjectPtr&) = default;
    TWeakObjectPtr(TWeakObjectPtr&&) = default;
    TWeakObjectPtr& operator=(TWeakObjectPtr&&) = default;

    TWeakObjectPtr(nullptr_t)
        : ObjectPtr(nullptr)
    {
    }

    TWeakObjectPtr(ElementType* InPtr)
        : ObjectPtr(InPtr)
    {
    }

    TWeakObjectPtr& operator=(nullptr_t)
    {
        ObjectPtr = nullptr;
        return *this;
    }

    TWeakObjectPtr& operator=(ElementType* InPtr)
    {
        ObjectPtr = InPtr;
        return *this;
    }

public:
    FORCEINLINE ElementType* Get() const
    {
        if (ObjectPtr)
        {
            // ReSharper disable once CppCStyleCast
            if (::IsValid((UObject*)ObjectPtr))
            {
                return ObjectPtr;
            }
            ObjectPtr = nullptr;
        }
        return nullptr;
    }

    FORCEINLINE ElementType* operator->() const { return Get(); }
    FORCEINLINE ElementType& operator*() const { return *Get(); }
    FORCEINLINE operator ElementType*() const { return Get(); }
    FORCEINLINE explicit operator bool() const { return Get() != nullptr; }

public:
    bool IsValid() const { return Get() != nullptr; }

private:
    mutable ElementType* ObjectPtr = nullptr;
};
