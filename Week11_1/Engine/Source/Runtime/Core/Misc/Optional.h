#pragma once
#include <cassert>
#include "HAL/PlatformType.h"


/**
 * Optional 값을 나타내는 템플릿 클래스입니다.
 * @tparam T 저장할 요소의 타입
 */
template <typename T>
struct TOptional
{
public:
    using ElementType = T;

    FORCEINLINE TOptional()
        : Storage{}
        , bIsSet(false)
    {
    }

    FORCEINLINE TOptional(const T& InValue)
        : bIsSet(false)
    {
        Construct(InValue);
    }

    FORCEINLINE TOptional(T&& InValue)
        : bIsSet(false)
    {
        Construct(std::move(InValue));
    }

    FORCEINLINE TOptional(const TOptional& Other)
        : bIsSet(false)
    {
        if (Other.IsSet())
        {
            Construct(Other.GetStoredValue());
        }
    }

    FORCEINLINE TOptional(TOptional&& Other) noexcept
        : bIsSet(false)
    {
        if (Other.IsSet())
        {
            Construct(std::move(Other.GetStoredValue()));
            Other.Reset();
        }
    }

    FORCEINLINE ~TOptional()
    {
        Reset();
    }

    FORCEINLINE TOptional& operator=(const T& InValue)
    {
        if (IsSet())
        {
            GetStoredValue() = InValue;
        }
        else
        {
            Construct(InValue);
        }
        return *this;
    }

    FORCEINLINE TOptional& operator=(T&& InValue)
    {
        if (IsSet())
        {
            GetStoredValue() = std::move(InValue);
        }
        else
        {
            Construct(std::move(InValue));
        }
        return *this;
    }

    FORCEINLINE TOptional& operator=(const TOptional& Other)
    {
        if (this != &Other)
        {
            Reset(); // 기존 값 안전하게 제거
            if (Other.IsSet())
            {
                Construct(Other.GetStoredValue());
            }
        }
        return *this;
    }

    FORCEINLINE TOptional& operator=(TOptional&& Other) noexcept
    {
        if (this != &Other)
        {
            Reset(); // 기존 값 안전하게 제거
            if (Other.IsSet())
            {
                Construct(std::move(Other.GetStoredValue()));
                Other.Reset(); // 이동 후 원본은 비움
            }
        }
        return *this;
    }

    /**
     * 값이 설정되어 있는지 확인합니다.
     * @return 값이 있으면 true, 없으면 false.
     */
    FORCEINLINE bool IsSet() const
    {
        return bIsSet;
    }

    /**
     * 값이 설정되어 있는지 확인합니다 (bool 변환).
     * @return 값이 있으면 true, 없으면 false.
     */
    FORCEINLINE explicit operator bool() const
    {
        return IsSet();
    }

    /**
     * 저장된 값에 대한 참조를 반환합니다.
     * @warning 값이 설정되어 있지 않으면 `assert` 실패로 프로그램이 중단됩니다.
     * @return 값에 대한 참조.
     */
    FORCEINLINE T& GetValue()
    {
        assert(IsSet()); // 값이 반드시 있어야 함
        return GetStoredValue();
    }

    /**
     * 저장된 값에 대한 const 참조를 반환합니다.
     * @warning 값이 설정되어 있지 않으면 `assert` 실패로 프로그램이 중단됩니다.
     * @return 값에 대한 const 참조.
     */
    FORCEINLINE const T& GetValue() const
    {
        assert(IsSet()); // 값이 반드시 있어야 함
        return GetStoredValue();
    }

    /**
     * 저장된 값에 대한 포인터를 반환합니다.
     * 값이 설정되어 있지 않으면 nullptr을 반환합니다.
     * @return 값에 대한 포인터 또는 nullptr.
     */
    FORCEINLINE T* Get()
    {
        return IsSet() ? &GetStoredValue() : nullptr;
    }

    /**
     * 저장된 값에 대한 const 포인터를 반환합니다.
     * 값이 설정되어 있지 않으면 nullptr을 반환합니다.
     * @return 값에 대한 const 포인터 또는 nullptr.
     */
    FORCEINLINE const T* Get() const
    {
        return IsSet() ? &GetStoredValue() : nullptr;
    }

    /**
     * 저장된 값에 대한 참조를 반환합니다 (역참조 연산자).
     * @warning 값이 설정되어 있지 않으면 `assert` 실패로 프로그램이 중단됩니다.
     * @return 값에 대한 참조.
     */
    FORCEINLINE T& operator*()
    {
        return GetValue();
    }

    /**
     * 저장된 값에 대한 const 참조를 반환합니다 (역참조 연산자).
     * @warning 값이 설정되어 있지 않으면 `assert` 실패로 프로그램이 중단됩니다.
     * @return 값에 대한 const 참조.
     */
    FORCEINLINE const T& operator*() const
    {
        return GetValue();
    }

    /**
     * 저장된 값의 멤버에 접근하기 위한 포인터를 반환합니다 (화살표 연산자).
     * @warning 값이 설정되어 있지 않으면 `assert` 실패로 프로그램이 중단됩니다.
     * @return 값에 대한 포인터.
     */
    FORCEINLINE T* operator->()
    {
        assert(IsSet());
        return &GetStoredValue();
    }

    /**
     * 저장된 값의 멤버에 접근하기 위한 const 포인터를 반환합니다 (화살표 연산자).
     * @warning 값이 설정되어 있지 않으면 `assert` 실패로 프로그램이 중단됩니다.
     * @return 값에 대한 const 포인터.
     */
    FORCEINLINE const T* operator->() const
    {
        assert(IsSet());
        return &GetStoredValue();
    }

    /**
     * 값을 초기화(제거)하여 비어있는 상태로 만듭니다.
     * 이미 비어있는 경우 아무 작업도 수행하지 않습니다.
     */
    FORCEINLINE void Reset()
    {
        if (IsSet())
        {
            Destroy();
            bIsSet = false;
        }
    }

    /**
     * 현재 TOptional이 값을 가지고 있다면 그 값을 반환하고, 그렇지 않다면 제공된 기본값을 반환합니다.
     * @param DefaultValue 값이 없을 경우 반환될 기본값 (lvalue 참조).
     * @return 저장된 값 또는 기본값.
     */
    template <typename U = T> // U는 T로 변환 가능해야 함 (SFINAE 등으로 더 엄격하게 제한 가능)
    FORCEINLINE T Get(const U& DefaultValue) const&
    {
        return IsSet() ? GetStoredValue() : static_cast<T>(DefaultValue);
    }

    /**
     * 현재 TOptional이 값을 가지고 있다면 그 값을 이동하여 반환하고, 그렇지 않다면 제공된 기본값을 이동하여 반환합니다.
     * @param DefaultValue 값이 없을 경우 반환될 기본값 (rvalue 참조).
     * @return 저장된 값 또는 기본값.
     */
    template <typename U = T>
    FORCEINLINE T Get(U&& DefaultValue)&&  // NOLINT(cppcoreguidelines-missing-std-forward)
    {
        return IsSet() ? std::move(GetStoredValue()) : static_cast<T>(std::move(DefaultValue));  // NOLINT(bugprone-move-forwarding-reference)
    }

    /**
     * 전달된 인자들을 사용하여 TOptional 내부에 직접 객체를 생성합니다.
     * 만약 이미 값이 존재한다면, 기존 값은 파괴됩니다.
     * @param Args 생성자 인자들.
     * @return 생성된 객체에 대한 참조.
     */
    template <typename... ArgsType>
    FORCEINLINE T& Emplace(ArgsType&&... Args)
    {
        Reset(); // 기존 값 파괴 및 bIsSet = false 처리
        // placement new를 사용하여 Storage에 객체 생성
        new (&Storage) T(std::forward<ArgsType>(Args)...);
        bIsSet = true;
        return GetStoredValue();
    }

private:
    // 내부 저장소. TTypeCompatibleBytes와 유사한 역할로,
    // 객체를 직접 저장하지 않고 바이트 배열로 공간을 확보한 뒤 placement new로 생성합니다.
    alignas(T) unsigned char Storage[sizeof(T)];
    bool bIsSet; // 값이 현재 설정되어 있는지 여부

    // 저장된 값에 직접 접근 (내부용, IsSet() 체크 없음)
    FORCEINLINE T& GetStoredValue()
    {
        // Storage를 T 타입의 포인터로 재해석하여 값에 접근
        return *reinterpret_cast<T*>(&Storage);  // NOLINT(clang-diagnostic-undefined-reinterpret-cast)
    }

    FORCEINLINE const T& GetStoredValue() const
    {
        return *reinterpret_cast<const T*>(&Storage);
    }

    // 값 생성 (내부용). 전달된 인자들을 사용하여 Storage에 객체를 생성합니다.
    template <typename... ArgsType>
    FORCEINLINE void Construct(ArgsType&&... Args)
    {
        assert(!bIsSet); // 이미 설정되어 있으면 안됨 (논리적 오류 방지)
        new (&Storage) T(std::forward<ArgsType>(Args)...); // placement new
        bIsSet = true;
    }

    // 값 파괴 (내부용). Storage에 생성된 객체의 소멸자를 명시적으로 호출합니다.
    FORCEINLINE void Destroy()
    {
        // IsSet() 체크는 Reset()에서 이미 수행하므로 여기서는 생략 가능하나, 안전을 위해 추가 가능
        // assert(bIsSet);
        GetStoredValue().~T(); // T의 소멸자 명시적 호출
    }

public:
    // --- 비교 연산자들 ---

    /** 두 TOptional 객체가 같은지 비교합니다. */
    friend FORCEINLINE bool operator==(const TOptional<T>& Lhs, const TOptional<T>& Rhs)
    {
        if (Lhs.IsSet() != Rhs.IsSet())
        {
            return false; // 하나는 설정되어 있고 다른 하나는 아니면 다름
        }
        if (!Lhs.IsSet()) // 둘 다 IsSet() == false 이면 (위 조건 통과 후)
        {
            return true;  // 둘 다 비어있으면 같음
        }
        return *Lhs == *Rhs; // 둘 다 값이 있으면 값 비교
    }

    /** 두 TOptional 객체가 다른지 비교합니다. */
    friend FORCEINLINE bool operator!=(const TOptional<T>& Lhs, const TOptional<T>& Rhs)
    {
        return !(Lhs == Rhs);
    }

    // TOptional 과 값(T) 비교
    friend FORCEINLINE bool operator==(const TOptional<T>& Lhs, const T& Rhs)
    {
        return Lhs.IsSet() ? (*Lhs == Rhs) : false;
    }
    friend FORCEINLINE bool operator==(const T& Lhs, const TOptional<T>& Rhs)
    {
        return Rhs.IsSet() ? (Lhs == *Rhs) : false;
    }
    friend FORCEINLINE bool operator!=(const TOptional<T>& Lhs, const T& Rhs)
    {
        return !(Lhs == Rhs);
    }
    friend FORCEINLINE bool operator!=(const T& Lhs, const TOptional<T>& Rhs)
    {
        return !(Lhs == Rhs);
    }

    // TODO: 필요한 경우 operator<, operator>, operator<=, operator>= 등 추가
    // TODO: FNullOptOptional (또는 이와 유사한 NullOpt 같은 타입) 지원 추가 가능
};

/**
 * 주어진 값으로 TOptional 객체를 생성하는 헬퍼 함수입니다. (Make 함수 스타일)
 * @param Value TOptional에 저장될 값 (lvalue 또는 rvalue).
 * @return 생성된 TOptional 객체. 타입 추론을 통해 ElementType이 결정됩니다.
 */
template <typename T>
FORCEINLINE TOptional<std::decay_t<T>> MakeOptional(T&& Value)
{
    return TOptional<std::decay_t<T>>(std::forward<T>(Value));
}
