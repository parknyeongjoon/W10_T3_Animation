#pragma once
#include "Object.h"      // UObject
#include "NameTypes.h"   // FName, NAME_None
#include "Container/Array.h" // TArray
#include "UObjectHash.h"   // GetObjectsOfClass 함수 선언
#include <cassert>         // assert

/**
 *
 * 지정된 Outer 내에서 특정 이름과 클래스(또는 파생 클래스)를 가진 UObject를 찾습니다.
 *
 * @template T 찾고자 하는 객체의 타입 (UObject 파생 클래스).
 * @param Outer 검색을 수행할 범위 객체. 이 객체의 직속 자식 중에서 검색합니다. nullptr은 특정 Outer에 속하지 않은 객체를 의미할 수 있으나, 이 구현에서는 Outer가 일치하는지만 확인합니다.
 * @param Name 찾고자 하는 객체의 이름 (TCHAR* 문자열).
 * @param bExactClass true이면 정확히 T 클래스만 찾고, false이면 T의 파생 클래스도 포함합니다.
 * @return 찾은 객체의 포인터 (T* 타입). 찾지 못하면 nullptr을 반환합니다.
 */
template <typename T>
T* FindObject(UObject* Outer, const TCHAR* Name, bool bExactClass = false)
{
    // 1. 입력 유효성 검사 및 준비
    assert(Name != nullptr && "FindObject called with null name string.");
    if (Name == nullptr || Name[0] == TEXT('\0'))
    {
        // 비어 있거나 null인 이름은 유효하지 않음
        return nullptr;
    }

    FName NameToFind(Name); // TCHAR* 를 FName으로 변환
    if (NameToFind == NAME_None)
    {
        // NAME_None 은 보통 유효한 객체 이름이 아님
        return nullptr;
    }

    // 템플릿 타입 T로부터 UClass* 가져오기
    UClass* ClassToFind = T::StaticClass();
    if (ClassToFind == nullptr)
    {
        // T가 유효한 UObject 클래스가 아님
        assert(false && "FindObject called with invalid template type T.");
        return nullptr;
    }

    // 2. 클래스 기반 후보 필터링 (FUObjectHashTables 활용)
    TArray<UObject*> CandidateObjects;
    // GetObjectsOfClass는 파생 클래스 포함 여부를 bIncludeDerivedClasses로 받으므로, bExactClass의 반대 값을 사용
    // 여기서는 함수 시그니처가 const를 받는다고 가정하고 진행.
    const UClass* ConstClassToFind = ClassToFind;
    GetObjectsOfClass(ConstClassToFind, CandidateObjects, !bExactClass);

    // 3. 후보 객체 순회하며 Outer 및 Name 확인
    for (UObject* Candidate : CandidateObjects)
    {
        // 후보 객체가 null이 아니고 (안전을 위해 확인),
        // Outer가 일치하고,
        // 이름(FName)이 일치하면
        if (Candidate != nullptr &&
            Candidate->GetOuter() == Outer &&
            Candidate->GetFName() == NameToFind)
        {
            // 찾았다! static_cast는 클래스 호환성이 GetObjectsOfClass에서
            // 이미 어느 정도 보장되었으므로 안전하게 사용 가능.
            return static_cast<T*>(Candidate);
        }
    }

    // 4. 모든 후보를 확인했지만 찾지 못함
    return nullptr;
}

/**
 * 지정된 Outer 내에서 특정 이름과 클래스(또는 파생 클래스)를 가진 UObject를 찾습니다. (FName 오버로드)
 *
 * @template T 찾고자 하는 객체의 타입 (UObject 파생 클래스).
 * @param Outer 검색을 수행할 범위 객체.
 * @param NameToFind 찾고자 하는 객체의 FName.
 * @param bExactClass true이면 정확히 T 클래스만 찾고, false이면 T의 파생 클래스도 포함합니다.
 * @return 찾은 객체의 포인터 (T* 타입). 찾지 못하면 nullptr을 반환합니다.
 */
template <typename T>
T* FindObject(UObject* Outer, FName NameToFind, bool bExactClass = false)
{

    // 1. 입력 유효성 검사 및 준비
    if (NameToFind == NAME_None)
    {
        return nullptr;
    }

    UClass* ClassToFind = T::StaticClass();
    if (ClassToFind == nullptr)
    {
        assert(false && "FindObject called with invalid template type T.");
        return nullptr;
    }

    // 2. 클래스 기반 후보 필터링
    TArray<UObject*> CandidateObjects;
    const UClass* ConstClassToFind = ClassToFind;
    GetObjectsOfClass(ConstClassToFind, CandidateObjects, !bExactClass);

    // 3. 후보 객체 순회하며 Outer 및 Name 확인
    for (UObject* Candidate : CandidateObjects)
    {
        if (Candidate != nullptr &&
            Candidate->GetOuter() == Outer &&
            Candidate->GetFName() == NameToFind)
        {
            return static_cast<T*>(Candidate);
        }
    }

    // 4. 찾지 못함
    return nullptr;
}
