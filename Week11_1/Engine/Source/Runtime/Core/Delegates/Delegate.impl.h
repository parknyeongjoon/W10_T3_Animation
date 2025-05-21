#pragma once

// 실제 사용하는 헤더 포함
#include "Delegate.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Actor.h"
#include "Actors/ADodge.h"

template <typename ReturnType, typename... ParamTypes>
void TMulticastDelegate<ReturnType(ParamTypes...)>::DuplicateMemberFunctionForType(
    const std::type_index& ObjectType, 
    UObject* DuplicatedObject,
    std::shared_ptr<IFunctionPointerBase> FuncPtr, 
    TMulticastDelegate& OutDelegate) const
{
    if (ObjectType == std::type_index(typeid(USceneComponent)))
    {
        DuplicateMemberFunctionForClass<USceneComponent>(DuplicatedObject, FuncPtr, OutDelegate);
    }
    else if (ObjectType == std::type_index(typeid(AActor)))
    {
        DuplicateMemberFunctionForClass<AActor>(DuplicatedObject, FuncPtr, OutDelegate);
    }
    else if (ObjectType == std::type_index(typeid(ADodge)))
    {
        DuplicateMemberFunctionForClass<ADodge>(DuplicatedObject, FuncPtr, OutDelegate);
    }
    else if (ObjectType == std::type_index(typeid(AStaticMeshActor)))
    {
        DuplicateMemberFunctionForClass<AStaticMeshActor>(DuplicatedObject, FuncPtr, OutDelegate);
    }
}

template <typename ReturnType, typename... ParamTypes>
template <typename T>
void TMulticastDelegate<ReturnType(ParamTypes...)>::DuplicateMemberFunctionForClass(
    UObject* DuplicatedObject, 
    std::shared_ptr<IFunctionPointerBase> FuncPtr, 
    TMulticastDelegate& OutDelegate) const
{
    // T 타입으로 함수 포인터 캐스팅
    auto CastedFuncPtr = static_cast<TMemberFunctionPointer<T, ReturnType, ParamTypes...>*>(FuncPtr.get());
    if (CastedFuncPtr)
    {
        // 함수 포인터 가져오기
        auto MemberFunction = CastedFuncPtr->GetFunctionPtr();
        
        // T 타입으로 객체 캐스팅 및 바인딩
        T* TypedObject = static_cast<T*>(DuplicatedObject);
        if (TypedObject)
        {
            OutDelegate.AddUObject(TypedObject, MemberFunction);
        }
    }
}