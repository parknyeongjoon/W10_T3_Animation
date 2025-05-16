#pragma once
#include "Container/Array.h"

class UObject;
class UClass;

/**
 * ClassToLookFor와 일치하는 UObject를 반환합니다.
 * @param ClassToLookFor 반환할 Object의 Class정보
 * @param Results ClassToLookFor와 일치하는 모든 Objects가 담길 목록
 * @param bIncludeDerivedClasses ClassToLookFor의 파생 클래스까지 찾을지 여부
 */
void GetObjectsOfClass(const UClass* ClassToLookFor, TArray<UObject*>& Results, bool bIncludeDerivedClasses);

/** FUObjectHashTables에 Object의 정보를 저장합니다. */
void AddToClassMap(UObject* Object);

/** FUObjectHashTables에 저장된 Object정보를 제거합니다. */
void RemoveFromClassMap(UObject* Object);
