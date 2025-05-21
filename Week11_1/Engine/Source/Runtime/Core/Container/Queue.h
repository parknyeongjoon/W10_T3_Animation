#pragma once
#include <deque>
#include "ContainerAllocator.h"


/**
 * 템플릿 기반으로 구현된 큐 자료 구조입니다. 내부적으로 std::deque을 이용하여 구현됩니다.
 * FIFO(선입선출) 방식으로 요소를 관리하며 동적 크기를 지원합니다.
 *
 * @tparam T 큐가 저장할 요소의 타입
 * @tparam Allocator 요소를 관리할 메모리 할당자
 */
template <typename T, typename Allocator = FDefaultAllocator<T>>
class TQueue
{
public:
    using ContainerType = std::deque<T, Allocator>;
    using SizeType = typename Allocator::SizeType;
    using ElementType = T;

private:
    ContainerType ContainerPrivate;

public:
    ContainerType& GetContainerPrivate() { return ContainerPrivate; }
    const ContainerType& GetContainerPrivate() const { return ContainerPrivate; }

    /** 기본 생성자. 비어 있는 큐를 생성합니다. */
    TQueue() = default;

    /** 소멸자 */
    ~TQueue() = default;

    TQueue(const TQueue&) = delete;
    TQueue& operator=(const TQueue&) = delete;

    TQueue(TQueue&&) noexcept = default;
    TQueue& operator=(TQueue&&) noexcept = default;

public:
    /**
     * 큐의 맨 뒤에 새 요소를 추가합니다 (복사).
     *
     * @param Item 추가할 요소
     * @return 성공적으로 추가되었으면 true (std::deque는 일반적으로 예외를 던지지 않는 한 성공)
     */
    bool Enqueue(const ElementType& Item)
    {
        ContainerPrivate.push_back(Item);
        return true; // std::deque::push_back은 성공 시 void 반환, 실패 시 예외. 여기서는 bool 반환 스타일 유지
    }

    /**
     * 큐의 맨 뒤에 새 요소를 추가합니다 (이동).
     *
     * @param Item 추가할 요소 (rvalue 참조)
     * @return 성공적으로 추가되었으면 true
     */
    bool Enqueue(ElementType&& Item)
    {
        ContainerPrivate.push_back(std::move(Item));
        return true;
    }

    /**
     * 큐의 맨 뒤에 새 요소를 직접 생성하여 추가합니다 (emplacement).
     * TArray의 Emplace와 유사하게 작동합니다.
     *
     * @tparam ArgsType 생성자 인자 타입들
     * @param Args 요소 생성에 필요한 인자들
     * @return 성공적으로 추가되었으면 true
     */
    template <typename... ArgsType>
    bool Emplace(ArgsType&&... Args)
    {
        ContainerPrivate.emplace_back(std::forward<ArgsType>(Args)...);
        return true;
    }

    /**
     * 큐의 맨 앞에서 요소를 제거하고 그 값을 반환합니다.
     * 큐가 비어있으면 false를 반환하고 OutItem은 변경되지 않습니다.
     *
     * @param OutItem 제거된 요소의 값을 저장할 변수 (출력 파라미터)
     * @return 요소가 성공적으로 제거되었으면 true, 큐가 비어있으면 false
     */
    bool Dequeue(ElementType& OutItem)
    {
        if (IsEmpty())
        {
            return false;
        }
        // front()는 예외를 던지지 않지만, pop_front()는 비어있을 때 정의되지 않은 동작일 수 있으므로 IsEmpty 체크 필수.
        // std::move를 사용하여 가능하면 이동 시맨틱을 활용합니다.
        OutItem = std::move(ContainerPrivate.front());
        ContainerPrivate.pop_front();
        return true;
    }

    /**
     * 큐의 맨 앞에서 요소를 제거합니다. (값을 반환하지 않음)
     * 큐가 비어있으면 아무 작업도 하지 않습니다.
     *
     * @return 요소를 제거했으면 true, 큐가 비어있으면 false
     */
    bool Dequeue()
    {
        if (IsEmpty())
        {
            return false;
        }
        ContainerPrivate.pop_front();
        return true;
    }

    /**
     * 큐의 맨 앞에 있는 요소를 제거하지 않고 그 값을 확인합니다.
     * 큐가 비어있으면 false를 반환하고 OutItem은 변경되지 않습니다.
     *
     * @param OutItem 맨 앞 요소의 값을 저장할 변수 (출력 파라미터)
     * @return 맨 앞 요소를 성공적으로 읽었으면 true, 큐가 비어있으면 false
     */
    bool Peek(ElementType& OutItem) const
    {
        if (IsEmpty())
        {
            return false;
        }
        // const 버전이므로 복사합니다.
        OutItem = ContainerPrivate.front();
        return true;
    }

    /**
     * 큐의 맨 앞에 있는 요소를 제거하지 않고 포인터를 반환합니다.
     * 큐가 비어있으면 nullptr을 반환합니다.
     * 주의: Dequeue나 Empty 호출 시 이 포인터는 유효하지 않게 됩니다.
     *
     * @return 맨 앞 요소에 대한 포인터, 큐가 비어있으면 nullptr
     */
    ElementType* Peek()
    {
        if (IsEmpty())
        {
            return nullptr;
        }
        return &ContainerPrivate.front();
    }

    /**
     * 큐의 맨 앞에 있는 요소를 제거하지 않고 const 포인터를 반환합니다. (const 버전)
     * 큐가 비어있으면 nullptr을 반환합니다.
     *
     * @return 맨 앞 요소에 대한 const 포인터, 큐가 비어있으면 nullptr
     */
    const ElementType* Peek() const
    {
        if (IsEmpty())
        {
            return nullptr;
        }
        return &ContainerPrivate.front();
    }

    /** 큐의 맨 앞에서 요소를 제거합니다. (값을 반환하지 않음) */
    bool Pop()
    {
        if (IsEmpty())
        {
            return false;
        }
        ContainerPrivate.pop_front();
        return true;
    }

public:
    /**
     * 큐가 비어 있는지 확인합니다.
     *
     * @return 큐가 비어있으면 true, 아니면 false
     */
    [[nodiscard]] bool IsEmpty() const
    {
        return ContainerPrivate.empty();
    }

    /** 큐의 모든 요소를 제거합니다. */
    void Empty()
    {
        ContainerPrivate.clear();
    }

    /**
     * 큐에 있는 요소의 개수를 반환합니다.
     *
     * @return 큐의 요소 개수
     */
    SizeType Num() const
    {
        return ContainerPrivate.size();
    }
};

// TQueue에 대한 직렬화 연산자 (선택 사항, TArray 예시처럼 구현)
// #include "Serialization/Archive.h" // 실제 사용 시 필요
/*
template <typename ElementType, typename Allocator>
FArchive& operator<<(FArchive& Ar, TQueue<ElementType, Allocator>& Queue)
{
    using SizeType = typename TQueue<ElementType, Allocator>::SizeType;

    if (Ar.IsLoading())
    {
        // 로딩 시: 먼저 큐를 비우고 크기를 읽음
        Queue.Empty();
        SizeType Count;
        Ar << Count;

        // 요소들을 순서대로 읽어서 Enqueue
        for (SizeType i = 0; i < Count; ++i)
        {
            ElementType Element;
            Ar << Element;
            Queue.Enqueue(std::move(Element)); // 읽은 후 이동
        }
    }
    else // 저장 시
    {
        // 크기 저장
        SizeType Count = Queue.Num();
        Ar << Count;

        // 내부 컨테이너에 접근하여 순서대로 저장 (주의: 임시 복사본 생성)
        // TQueue 자체는 직접 인덱싱/이터레이터를 제공하지 않으므로,
        // GetContainerPrivate를 사용하거나, Dequeue/Enqueue를 반복해야 함.
        // GetContainerPrivate가 더 효율적.
        const auto& Container = Queue.GetContainerPrivate();
        for (const auto& Element : Container)
        {
            Ar << Element;
        }
        // 또는, 큐를 복사해서 Dequeue 하는 방식 (원본 보존)
        // TQueue<ElementType, Allocator> TempQueue = Queue;
        // while (!TempQueue.IsEmpty())
        // {
        //     ElementType Element;
        //     TempQueue.Dequeue(Element);
        //     Ar << Element;
        // }
    }
    return Ar;
}
*/
