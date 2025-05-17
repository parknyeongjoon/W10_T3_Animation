#pragma once
#include <algorithm>
#include <utility>
#include <vector>

#include "ContainerAllocator.h"
#include "Serialization/Archive.h"


template <typename T, typename AllocatorType = FDefaultAllocator<T>>
class TArray
{
public:
    using SizeType = typename AllocatorType::SizeType;
    using ElementType = T;
    using ArrayType = std::vector<ElementType, AllocatorType>;

private:
    ArrayType ContainerPrivate;

public:
    // Iterator를 사용하기 위함
    auto begin() noexcept { return ContainerPrivate.begin(); }
    auto end() noexcept { return ContainerPrivate.end(); }
    auto begin() const noexcept { return ContainerPrivate.begin(); }
    auto end() const noexcept { return ContainerPrivate.end(); }
    auto rbegin() noexcept { return ContainerPrivate.rbegin(); }
    auto rend() noexcept { return ContainerPrivate.rend(); }
    auto rbegin() const noexcept { return ContainerPrivate.rbegin(); }
    auto rend() const noexcept { return ContainerPrivate.rend(); }

    T& operator[](SizeType Index);
    const T& operator[](SizeType Index) const;
	void operator+(const TArray& OtherArray);

public:
    TArray();
    ~TArray() = default;

    // 이니셜라이저 생성자
    TArray(std::initializer_list<T> InitList);

    // 복사 생성자
    TArray(const TArray& Other);

    // 이동 생성자
    TArray(TArray&& Other) noexcept;

    // 복사 할당 연산자
    TArray& operator=(const TArray& Other);

    // 이동 할당 연산자
    TArray& operator=(TArray&& Other) noexcept;

	/** Element를 Number개 만큼 초기화 합니다. */
    void Init(const T& Element, SizeType Number);
    SizeType Add(const T& Item);
    SizeType Add(T&& Item);
    SizeType AddUnique(const T& Item);

	template <typename... Args>
    SizeType Emplace(Args&&... Item);

    /** Array가 비어있는지 확인합니다. */
    bool IsEmpty() const;

	/** Array를 비웁니다 */
    void Empty();

	/** Item과 일치하는 모든 요소를 제거합니다. */
    SizeType Remove(const T& Item);

	/** 왼쪽부터 Item과 일치하는 요소를 1개 제거합니다. */
    bool RemoveSingle(const T& Item);

	/** 특정 위치에 있는 요소를 제거합니다. */
    void RemoveAt(SizeType Index);

	/** Predicate에 부합하는 모든 요소를 제거합니다. */
    template <typename Predicate>
        requires std::is_invocable_r_v<bool, Predicate, const T&>
    SizeType RemoveAll(const Predicate& Pred);

    T* GetData();
    const T* GetData() const;

    /**
     * Array에서 Item을 찾습니다.
     * @param Item 찾으려는 Item
     * @return Item의 인덱스, 찾을 수 없다면 -1
     */
    SizeType Find(const T& Item);
    bool Find(const T& Item, SizeType& Index);

    /** 요소가 존재하는지 확인합니다. */
    bool Contains(const T& Item) const;

    /** Array Size를 가져옵니다. */
    SizeType Num() const;

    /** Array의 Capacity를 가져옵니다. */
    SizeType Len() const;

	/** Array의 Size를 Number로 설정합니다. */
	void SetNum(SizeType Number);

	/** Array의 Capacity를 Number로 설정합니다. */
    void Reserve(SizeType Number);

    void Sort();
    template <typename Compare>
        requires std::is_invocable_r_v<bool, Compare, const T&, const T&>
    void Sort(const Compare& CompFn);


    /**
 * 다른 TArray의 모든 요소를 이 배열의 끝에 추가합니다.
 * @param Source 다른 TArray 객체
 */
    void Append(const TArray& Source);

    /**
     * InitList에 있는 요소들을 InIndex 위치에 삽입합니다.
     * @param InitList 삽입할 요소들의 초기화 리스트
     * @param InIndex 삽입 시작 위치의 인덱스
     * @return 삽입된 첫 번째 요소의 인덱스
     */
    SizeType Insert(std::initializer_list<ElementType> InitList, SizeType InIndex);

    /**
     * 다른 TArray의 요소들을 현재 배열의 특정 위치에 삽입합니다.
     *
     * @param Items 삽입할 요소들이 포함된 TArray
     * @param InIndex 요소들을 삽입할 인덱스 위치
     * @return 삽입된 첫 번째 요소의 인덱스
     */
    template <typename OtherAllocatorType>
    SizeType Insert(const TArray<ElementType, OtherAllocatorType>& Items, SizeType InIndex);

    /**
     * 하나의 요소를 특정 인덱스 위치에 삽입합니다.
     *
     * @param Item 삽입할 요소
     * @param InIndex 삽입 위치의 인덱스
     * @return 삽입된 요소의 인덱스
     */
    SizeType Insert(const ElementType& Item, SizeType InIndex);

    /**
     * 포인터가 가리키는 C-스타일 배열의 요소들을 이 배열의 끝에 추가합니다.
     * @param Ptr 추가할 요소 배열의 시작 포인터
     * @param Count 추가할 요소의 개수
     */
    void Append(const ElementType* Ptr, SizeType Count);


    void AppendArray(const T* array, SizeType count);

    /**
 * Count만큼 초기화되지 않은 공간을 확장합니다.
 * @warning std::vector의 한계로, 실제로는 AddDefaulted와 동작이 같습니다.
 */
    SizeType AddUninitialized(SizeType Count);

    /**
     * 배열 끝에 기본 생성된 요소 1개를 추가합니다.
     * @return 추가된 요소의 인덱스
     */
    SizeType AddDefaulted();

    /**
     * 배열 끝에 기본 생성된 요소를 Count개 만큼 추가합니다.
     * @param Count 추가할 요소의 개수
     * @return 추가된 첫 번째 요소의 인덱스. Count가 0 이하라면 현재 Num()을 반환할 수 있습니다.
     */
    SizeType AddDefaulted(SizeType Count);

    bool IsValidIndex(uint32 ElementIndex) const {
        if (ElementIndex < 0 || ElementIndex >= Num()) return false;

        return true;
    }

    void Serialize(FArchive& Ar) const
    {
        Ar << ContainerPrivate;
    }
    void Deserialize(FArchive& Ar)
    {
        Ar >> ContainerPrivate;
    }
};


template <typename T, typename Allocator>
T& TArray<T, Allocator>::operator[](SizeType Index)
{
    return ContainerPrivate[Index];
}

template <typename T, typename Allocator>
const T& TArray<T, Allocator>::operator[](SizeType Index) const
{
    return ContainerPrivate[Index];
}

template <typename T, typename Allocator>
void TArray<T, Allocator>::operator+(const TArray& OtherArray)
{
	ContainerPrivate.insert(end(), OtherArray.begin(), OtherArray.end());
}

template <typename T, typename Allocator>
TArray<T, Allocator>::TArray()
    : ContainerPrivate()
{
}

template <typename T, typename Allocator>
TArray<T, Allocator>::TArray(std::initializer_list<T> InitList)
    : ContainerPrivate(InitList)
{
}

template <typename T, typename Allocator>
TArray<T, Allocator>::TArray(const TArray& Other)
    : ContainerPrivate(Other.ContainerPrivate)
{
}

template <typename T, typename Allocator>
TArray<T, Allocator>::TArray(TArray&& Other) noexcept
    : ContainerPrivate(std::move(Other.ContainerPrivate))
{
}

template <typename T, typename Allocator>
TArray<T, Allocator>& TArray<T, Allocator>::operator=(const TArray& Other)
{
    if (this != &Other)
    {
        ContainerPrivate = Other.ContainerPrivate;
    }
    return *this;
}

template <typename T, typename Allocator>
TArray<T, Allocator>& TArray<T, Allocator>::operator=(TArray&& Other) noexcept
{
    if (this != &Other)
    {
        ContainerPrivate = std::move(Other.ContainerPrivate);
    }
    return *this;
}

template <typename T, typename Allocator>
void TArray<T, Allocator>::Init(const T& Element, SizeType Number)
{
    ContainerPrivate.assign(Number, Element);
}

template <typename T, typename Allocator>
typename TArray<T, Allocator>::SizeType TArray<T, Allocator>::Add(const T& Item)
{
    return Emplace(Item);
}

template <typename T, typename Allocator>
typename TArray<T, Allocator>::SizeType TArray<T, Allocator>::Add(T&& Item)
{
    return Emplace(std::move(Item));
}

template <typename T, typename Allocator>
typename TArray<T, Allocator>::SizeType TArray<T, Allocator>::AddUnique(const T& Item)
{
    if (SizeType Index; Find(Item, Index))
    {
        return Index;
    }
    return Add(Item);
}

template <typename T, typename Allocator>
template <typename... Args>
typename TArray<T, Allocator>::SizeType TArray<T, Allocator>::Emplace(Args&&... Item)
{
    ContainerPrivate.emplace_back(std::forward<Args>(Item)...);
    return Num()-1;
}

template <typename T, typename Allocator>
bool TArray<T, Allocator>::IsEmpty() const
{
    return ContainerPrivate.empty();
}

template <typename T, typename Allocator>
void TArray<T, Allocator>::Empty()
{
    ContainerPrivate.clear();
}

template <typename T, typename Allocator>
typename TArray<T, Allocator>::SizeType TArray<T, Allocator>::Remove(const T& Item)
{
    auto oldSize = ContainerPrivate.size();
    ContainerPrivate.erase(std::remove(ContainerPrivate.begin(), ContainerPrivate.end(), Item), ContainerPrivate.end());
    return static_cast<SizeType>(oldSize - ContainerPrivate.size());
}

template <typename T, typename Allocator>
bool TArray<T, Allocator>::RemoveSingle(const T& Item)
{
    auto it = std::find(ContainerPrivate.begin(), ContainerPrivate.end(), Item);
    if (it != ContainerPrivate.end())
    {
        ContainerPrivate.erase(it);
        return true;
    }
    return false;
}

template <typename T, typename Allocator>
void TArray<T, Allocator>::RemoveAt(SizeType Index)
{
    if (Index >= 0 && static_cast<SizeType>(Index) < ContainerPrivate.size())
    {
        ContainerPrivate.erase(ContainerPrivate.begin() + Index);
    }
}

template <typename T, typename Allocator>
template <typename Predicate>
    requires std::is_invocable_r_v<bool, Predicate, const T&>
typename TArray<T, Allocator>::SizeType TArray<T, Allocator>::RemoveAll(const Predicate& Pred)
{
    auto oldSize = ContainerPrivate.size();
    ContainerPrivate.erase(std::remove_if(ContainerPrivate.begin(), ContainerPrivate.end(), Pred), ContainerPrivate.end());
    return static_cast<SizeType>(oldSize - ContainerPrivate.size());
}

template <typename T, typename Allocator>
T* TArray<T, Allocator>::GetData()
{
    return ContainerPrivate.data();
}

template <typename T, typename Allocator>
const T* TArray<T, Allocator>::GetData() const
{
    return ContainerPrivate.data();
}

template <typename T, typename Allocator>
typename TArray<T, Allocator>::SizeType TArray<T, Allocator>::Find(const T& Item)
{
    const auto it = std::find(ContainerPrivate.begin(), ContainerPrivate.end(), Item);
    return it != ContainerPrivate.end() ? std::distance(ContainerPrivate.begin(), it) : -1;
}

template <typename T, typename Allocator>
bool TArray<T, Allocator>::Find(const T& Item, SizeType& Index)
{
    Index = Find(Item);
    return (Index != -1);
}

template <typename T, typename Allocator>
bool TArray<T, Allocator>::Contains(const T& Item) const
{
    for (const T* Data = GetData(), *DataEnd = Data + Num(); Data != DataEnd; ++Data)
    {
        if (*Data == Item)
        {
            return true;
        }
    }
    return false;
}

template <typename T, typename Allocator>
typename TArray<T, Allocator>::SizeType TArray<T, Allocator>::Num() const
{
    return ContainerPrivate.size();
}

template <typename T, typename Allocator>
typename TArray<T, Allocator>::SizeType TArray<T, Allocator>::Len() const
{
    return ContainerPrivate.capacity();
}

template <typename T, typename Allocator>
void TArray<T, Allocator>::SetNum(SizeType Number)
{
	ContainerPrivate.resize(Number);
}

template <typename T, typename Allocator>
void TArray<T, Allocator>::Reserve(SizeType Number)
{
    ContainerPrivate.reserve(Number);
}

template <typename T, typename Allocator>
void TArray<T, Allocator>::Sort()
{
    std::sort(ContainerPrivate.begin(), ContainerPrivate.end());
}

template <typename T, typename AllocatorType>
void TArray<T, AllocatorType>::Append(const TArray& Source)
{
    // 추가할 요소가 없으면 바로 반환
    if (Source.IsEmpty())
    {
        return;
    }

    // 최적화: 필요한 경우 미리 메모리를 할당하여 여러 번의 재할당 방지
    const SizeType OldSize = Num();
    const SizeType NumToAdd = Source.Num();
    const SizeType NewSize = OldSize + NumToAdd;
    if (Len() < NewSize)
    {
        Reserve(NewSize); // 필요한 만큼 (또는 그 이상) 용량 확보
    }

    // std::vector::insert를 사용하여 Source의 모든 요소를 현재 벡터의 끝(end())에 삽입
    ContainerPrivate.insert(
        ContainerPrivate.end(),          // 삽입 위치: 현재 벡터의 끝
        Source.ContainerPrivate.begin(), // 복사할 시작 이터레이터
        Source.ContainerPrivate.end()    // 복사할 끝 이터레이터
    );
}
template <typename T, typename AllocatorType>
typename TArray<T, AllocatorType>::SizeType TArray<T, AllocatorType>::Insert(std::initializer_list<ElementType> InitList, const SizeType InIndex)
{
    auto InsertPosIter = ContainerPrivate.begin() + InIndex;
    ContainerPrivate.insert(InsertPosIter, InitList);
    return InIndex;
}

template <typename T, typename AllocatorType>
template <typename OtherAllocatorType>
typename TArray<T, AllocatorType>::SizeType TArray<T, AllocatorType>::Insert(
    const TArray<ElementType, OtherAllocatorType>& Items, const SizeType InIndex
)
{
    auto InsertPosIter = ContainerPrivate.begin() + InIndex;
    ContainerPrivate.insert(InsertPosIter, Items.begin(), Items.end());
    return InIndex;
}

template <typename T, typename AllocatorType>
typename TArray<T, AllocatorType>::SizeType TArray<T, AllocatorType>::Insert(const ElementType& Item, SizeType InIndex)
{
    auto InsertPosIter = ContainerPrivate.begin() + InIndex;
    ContainerPrivate.insert(InsertPosIter, Item);
    return InIndex;
}

template <typename T, typename AllocatorType>
void TArray<T, AllocatorType>::Append(const ElementType* Ptr, SizeType Count)
{
    // 추가할 요소가 없거나 포인터가 유효하지 않으면 바로 반환
    if (Count <= 0)
    {
        return;
    }
    // Count가 0보다 클 때 Ptr이 nullptr이면 문제가 발생하므로 확인 (assert 또는 예외 처리 등)
    assert(Ptr != nullptr && "TArray::Append trying to append from null pointer with Count > 0");
    if (Ptr == nullptr) {
        // 실제 엔진이라면 로그를 남기거나 할 수 있음
        return;
    }


    // 최적화: 필요한 경우 미리 메모리를 할당
    const SizeType OldSize = Num();
    const SizeType NewSize = OldSize + Count;
    if (Len() < NewSize)
    {
        Reserve(NewSize);
    }

    // std::vector::insert는 포인터를 이터레이터처럼 사용할 수 있음
    ContainerPrivate.insert(
        ContainerPrivate.end(), // 삽입 위치: 현재 벡터의 끝
        Ptr,                  // 복사할 시작 포인터 (이터레이터 역할)
        Ptr + Count           // 복사할 끝 포인터 (이터레이터 역할)
    );
}
template <typename T, typename Allocator>
template <typename Compare>
    requires std::is_invocable_r_v<bool, Compare, const T&, const T&>
void TArray<T, Allocator>::Sort(const Compare& CompFn)
{
    std::sort(ContainerPrivate.begin(), ContainerPrivate.end(), CompFn);
}

template <typename T, typename Allocator>
void TArray<T, Allocator>::AppendArray(const T* array, SizeType count)
{
    if (array && count > 0)
    {
        ContainerPrivate.insert(ContainerPrivate.end(), array, array + count);
    }
}


template <typename T, typename AllocatorType>
typename TArray<T, AllocatorType>::SizeType TArray<T, AllocatorType>::AddUninitialized(SizeType Count)
{
    if (Count <= 0)
    {
        return ContainerPrivate.size();
    }

    // 기존 크기 저장
    SizeType StartIndex = ContainerPrivate.size();

    // 메모리를 확장
    ContainerPrivate.resize(StartIndex + Count);

    // 새 크기를 반환
    return StartIndex;
}

template <typename T, typename AllocatorType>
typename TArray<T, AllocatorType>::SizeType TArray<T, AllocatorType>::AddDefaulted()
{
    // 새 요소들이 시작될 인덱스 (현재 크기)
    const SizeType StartIndex = Num();
    ContainerPrivate.emplace_back();
    return StartIndex;
}

template <typename T, typename AllocatorType>
typename TArray<T, AllocatorType>::SizeType TArray<T, AllocatorType>::AddDefaulted(SizeType Count)
{
    if (Count <= 0)
    {
        return Num();
    }

    // 새 요소들이 시작될 인덱스 (현재 크기)
    const SizeType StartIndex = Num();

    // resize를 사용하여 Count만큼 크기를 늘립니다.
    ContainerPrivate.resize(StartIndex + Count);

    // 추가된 첫 번째 요소의 인덱스 반환
    return StartIndex;
}


template <typename T> constexpr bool TIsTArray_V = false;

template <typename InElementType, typename InAllocatorType> constexpr bool TIsTArray_V<               TArray<InElementType, InAllocatorType>> = true;
template <typename InElementType, typename InAllocatorType> constexpr bool TIsTArray_V<const          TArray<InElementType, InAllocatorType>> = true;
template <typename InElementType, typename InAllocatorType> constexpr bool TIsTArray_V<      volatile TArray<InElementType, InAllocatorType>> = true;
template <typename InElementType, typename InAllocatorType> constexpr bool TIsTArray_V<const volatile TArray<InElementType, InAllocatorType>> = true;

template <typename T>
concept TIsTArray = TIsTArray_V<T>;
