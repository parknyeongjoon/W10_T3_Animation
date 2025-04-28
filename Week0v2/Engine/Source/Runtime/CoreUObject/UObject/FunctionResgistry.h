#pragma once
#include <functional>
#include <type_traits>
#include <typeindex>
#include "Core/Container/Map.h"
#include "UObject/Object.h"
#include "Delegates/Delegate.h"

// 함수 파라미터 패킹 및 언패킹을 위한 유틸리티
template<typename... Args>
struct TParamPack
{
    TParamPack(Args&&... InArgs) : Values(std::forward<Args>(InArgs)...) {}
    std::tuple<Args...> Values;
    
    // 튜플에서 파라미터를 언패킹하여 함수 호출에 사용
    template<typename Func, size_t... I>
    static auto CallWithParams(Func&& Function, const std::tuple<Args...>& Params, std::index_sequence<I...>)
    {
        return Function(std::get<I>(Params)...);
    }
    
    template<typename Func>
    static auto ApplyTuple(Func&& F, const std::tuple<Args...>& Params)
    {
        return CallWithParams(std::forward<Func>(F), Params, std::make_index_sequence<sizeof...(Args)>{});
    }
};

// 함수 레지스트리 항목을 위한 기본 인터페이스
class IFunctionRegistryItem
{
public:
    virtual ~IFunctionRegistryItem() = default;
    virtual bool Call(UObject* Object, void* Params) = 0;
    virtual std::vector<std::type_index> GetParamTypes() const = 0;
    virtual std::shared_ptr<IFunctionPointerBase> GetFunctionPointer() const = 0;
};

// 가변 파라미터를 지원하는 함수 레지스트리 항목
template<typename ReturnType, typename... ParamTypes>
class TVariadicFunctionRegistryItem : public IFunctionRegistryItem
{
public:
    using ParamPackType = TParamPack<ParamTypes...>;
    using FuncType = std::function<ReturnType(UObject*, ParamTypes...)>;
    
    TVariadicFunctionRegistryItem(FuncType InFunction) : Function(InFunction) {}
    
    bool Call(UObject* Object, void* Params) override
    {
        if (!Object || !Params) return false;
        
        ParamPackType* TypedParams = static_cast<ParamPackType*>(Params);
        
        // 튜플의 값들을 언패킹하여 함수 호출
        auto CallFunc = [this, Object](ParamTypes... Args) -> ReturnType {
            return Function(Object, std::forward<ParamTypes>(Args)...);
        };
        
        ParamPackType::ApplyTuple(CallFunc, TypedParams->Values);
        return true;
    }
    
    std::vector<std::type_index> GetParamTypes() const override
    {
        return {std::type_index(typeid(ParamTypes))...};
    }
    
    std::shared_ptr<IFunctionPointerBase> GetFunctionPointer() const override
    {
        // 델리게이트 시스템과 호환성을 위한 더미 함수 포인터
        // 실제 구현에서는 해당 함수의 실제 포인터를 반환해야 함
        return nullptr;
    }
    
private:
    FuncType Function;
};

// 가변 파라미터를 지원하는 멤버 함수 레지스트리 항목
template<typename ClassType, typename ReturnType, typename... ParamTypes>
class TVariadicMemberFunctionRegistryItem : public IFunctionRegistryItem
{
public:
    using ParamPackType = TParamPack<ParamTypes...>;
    using FuncPtrType = ReturnType(ClassType::*)(ParamTypes...);
    
    TVariadicMemberFunctionRegistryItem(FuncPtrType InFunction) : MemberFunction(InFunction)
    {
        FuncPtr = std::make_shared<TMemberFunctionPointer<ClassType, ReturnType, ParamTypes...>>(MemberFunction);
    }
    
    bool Call(UObject* Object, void* Params) override
    {
        if (!Object || !Params) return false;
        
        ClassType* TypedObject = static_cast<ClassType*>(Object);
        if (!TypedObject) return false;
        
        ParamPackType* TypedParams = static_cast<ParamPackType*>(Params);
        
        // 튜플의 값들을 언패킹하여 멤버 함수 호출
        auto CallMemberFunc = [this, TypedObject](ParamTypes... Args) -> ReturnType {
            return (TypedObject->*MemberFunction)(std::forward<ParamTypes>(Args)...);
        };
        
        ParamPackType::ApplyTuple(CallMemberFunc, TypedParams->Values);
        return true;
    }
    
    std::vector<std::type_index> GetParamTypes() const override
    {
        return {std::type_index(typeid(ParamTypes))...};
    }
    
    std::shared_ptr<IFunctionPointerBase> GetFunctionPointer() const override
    {
        return FuncPtr;
    }
    
private:
    FuncPtrType MemberFunction;
    std::shared_ptr<IFunctionPointerBase> FuncPtr;
};

// 함수 레지스트리 클래스
class FFunctionRegistry
{
public:
    // 가변 인자 람다 함수 등록
    template<typename ReturnType = void, typename... ParamTypes, typename FunctorType>
    bool RegisterFunction(FName FunctionName, FunctorType&& Functor)
    {
        auto RegistryItem = std::make_shared<TVariadicFunctionRegistryItem<ReturnType, ParamTypes...>>(
            [Func = std::forward<FunctorType>(Functor)](UObject* Obj, ParamTypes... Params) -> ReturnType {
                return Func(Obj, std::forward<ParamTypes>(Params)...);
            }
        );
        
        Functions.Add(FunctionName, RegistryItem);
        return true;
    }
    
    // 가변 인자 멤버 함수 등록
    template<typename ClassType, typename ReturnType = void, typename... ParamTypes>
    bool RegisterMemberFunction(FName FunctionName, ReturnType(ClassType::*MemberFunc)(ParamTypes...))
    {
        auto RegistryItem = std::make_shared<TVariadicMemberFunctionRegistryItem<ClassType, ReturnType, ParamTypes...>>(MemberFunc);
        Functions.Add(FunctionName, RegistryItem);
        return true;
    }
    
    // 가변 인자 함수 호출
    template<typename... ParamTypes>
    bool CallFunction(UObject* Object, FName FunctionName, ParamTypes&&... Params)
    {
        auto* ItemPtr = Functions.Find(FunctionName);
        if (!ItemPtr || !(*ItemPtr)) return false;
        
        auto& Item = *ItemPtr;
        
        // 파라미터 팩을 생성하여 함수에 전달
        using ParamPackType = TParamPack<std::decay_t<ParamTypes>...>;
        ParamPackType ParamPack(std::forward<ParamTypes>(Params)...);
        
        // 호출
        return Item->Call(Object, &ParamPack);
    }
    
    // 델리게이트에 함수 바인딩 (가변 인자)
    template<typename DelegateType, typename... ParamTypes>
    bool BindFunctionToDelegate(UObject* Object, FName FunctionName, DelegateType& OutDelegate)
    {
        auto* ItemPtr = Functions.Find(FunctionName);
        if (!ItemPtr || !(*ItemPtr)) return false;
        
        // 델리게이트에 바인드할 람다 생성
        OutDelegate.BindLambda([this, Object, FunctionName](ParamTypes... Params) {
            this->CallFunction(Object, FunctionName, std::forward<ParamTypes>(Params)...);
        });
        
        return true;
    }
    
    // 멀티캐스트 델리게이트에 함수 추가 (가변 인자)
    template<typename DelegateType, typename... ParamTypes>
    FDelegateHandle AddFunctionToMulticastDelegate(UObject* Object, FName FunctionName, DelegateType& OutDelegate)
    {
        auto* ItemPtr = Functions.Find(FunctionName);
        if (!ItemPtr || !(*ItemPtr)) return FDelegateHandle::CreateInvalidHandle();
        
        // 멀티캐스트 델리게이트에 람다 추가
        return OutDelegate.AddLambda([this, Object, FunctionName](ParamTypes... Params) {
            this->CallFunction(Object, FunctionName, std::forward<ParamTypes>(Params)...);
        });
    }

    TMap<FName, std::shared_ptr<IFunctionRegistryItem>> GetRegisteredFunctions()
    {
        return Functions;
    }
    
private:
    TMap<FName, std::shared_ptr<IFunctionRegistryItem>> Functions;
};