#pragma once
#include <functional>
#include <type_traits>
#include <typeindex>
#include "Core/Container/Map.h"

#include "Core/Container/Array.h"
#include "UObject/Object.h"

#define FUNC_DECLARE_DELEGATE(DelegateName, ReturnType, ...) \
	using DelegateName = TDelegate<ReturnType(__VA_ARGS__)>;

#define FUNC_DECLARE_MULTICAST_DELEGATE(MulticastDelegateName, ReturnType, ...) \
	using MulticastDelegateName = TMulticastDelegate<ReturnType(__VA_ARGS__)>;

// 델리게이트 핸들 클래스
class FDelegateHandle
{
	friend class std::hash<FDelegateHandle>;

	uint64 HandleId;
    
	explicit FDelegateHandle() : HandleId(0) {}
	explicit FDelegateHandle(uint64 HandleId) : HandleId(HandleId) {}

	static uint64 GenerateNewID()
	{
		static std::atomic<uint64> NextHandleId = 1;
		uint64 Result = NextHandleId.fetch_add(1, std::memory_order_relaxed);
		if (Result == 0) { Result = NextHandleId.fetch_add(1, std::memory_order_relaxed); }
		return Result;
	}

public:
	static FDelegateHandle CreateHandle() { return FDelegateHandle{GenerateNewID()}; }
    static FDelegateHandle CreateInvalidHandle() { return FDelegateHandle{0}; }

	bool IsValid() const { return HandleId != 0; }
	void Invalidate() { HandleId = 0; }

	bool operator==(const FDelegateHandle& Other) const { return HandleId == Other.HandleId; }
	bool operator!=(const FDelegateHandle& Other) const { return HandleId != Other.HandleId; }
};

// 타입 안전한 함수 포인터 저장을 위한 기본 클래스
class IFunctionPointerBase
{
public:
    virtual ~IFunctionPointerBase() = default;
    virtual bool IsValid() const = 0;
    // 타입 ID를 반환 (RTTI 활용)
    virtual std::type_index GetClassTypeIndex() const = 0;
};

// 특정 클래스와 시그니처에 대한 멤버 함수 포인터를 저장하는 클래스
template <typename ClassType, typename ReturnType, typename... ParamTypes>
class TMemberFunctionPointer : public IFunctionPointerBase
{
public:
    using FuncPtrType = ReturnType(ClassType::*)(ParamTypes...);
    
private:
    FuncPtrType FunctionPtr;
    
public:
    TMemberFunctionPointer(FuncPtrType InFunctionPtr) : FunctionPtr(InFunctionPtr) {}
    
    bool IsValid() const override { return FunctionPtr != nullptr; }
    std::type_index GetClassTypeIndex() const override { return std::type_index(typeid(ClassType)); }
    
    FuncPtrType GetFunctionPtr() const { return FunctionPtr; }
};

// 델리게이트 타입 정의
enum class EDelegateType : uint8
{
    None,
    MemberFunction,
    StaticFunction,
    LambdaFunction,
};

// 단일 델리게이트 클래스
template <typename Signature>
class TDelegate;

template <typename ReturnType, typename... ParamTypes>
class TDelegate<ReturnType(ParamTypes...)>
{
public:
    using FuncType = std::function<ReturnType(ParamTypes...)>;
    
private:
    FuncType Func;
    UObject* BoundObject = nullptr;
    EDelegateType Type = EDelegateType::None;
    std::shared_ptr<IFunctionPointerBase> FunctionPointer;

public:
    TDelegate() = default;
    
    // UObject에 멤버 함수 바인딩
    template <typename T, typename FuncPtrType>
        requires std::derived_from<T, UObject>
    void BindUObject(T* Object, FuncPtrType MemberFunction)
    {
        if (!Object || Object->GetName() == "Destroyed")
        {
            UnBind();
            return;
        }

        // 함수 포인터 저장
        FunctionPointer = std::make_shared<TMemberFunctionPointer<T, ReturnType, ParamTypes...>>(MemberFunction);
        BoundObject = Object;
        Type = EDelegateType::MemberFunction;
        
        // 함수 래퍼 생성
        Func = [ObjectPtr = Object, Function = MemberFunction](ParamTypes... Params) -> ReturnType
        {
            T* CurrentObject = static_cast<T*>(ObjectPtr);
            if (CurrentObject && CurrentObject->GetName() != "Destroyed")
            {
                return (CurrentObject->*Function)(std::forward<ParamTypes>(Params)...);
            }
            return ReturnType();
        };
    }

    // 정적 함수 바인딩
    template <typename FunctorType>
    void BindStatic(FunctorType&& Functor)
    {
        FunctionPointer = nullptr;
        BoundObject = nullptr;
        Type = EDelegateType::StaticFunction;
        
        Func = [StaticFunction = std::forward<FunctorType>(Functor)](ParamTypes... Params) -> ReturnType
        {
            return StaticFunction(std::forward<ParamTypes>(Params)...);
        };
    }
    
    // 람다 함수 바인딩
    template <typename FunctorType>
    void BindLambda(FunctorType&& InFunctor)
    {
        FunctionPointer = nullptr;
        BoundObject = nullptr;
        Type = EDelegateType::LambdaFunction;
        
        Func = [Func = std::forward<FunctorType>(InFunctor)](ParamTypes... Params)
        {
            return Func(std::forward<ParamTypes>(Params)...);
        };
    }

    // 바인딩 해제
    void UnBind()
    {
        Func = nullptr;
        BoundObject = nullptr;
        Type = EDelegateType::None;
        FunctionPointer = nullptr;
    }

    // 바인딩 여부 확인
    bool IsBound() const { return static_cast<bool>(Func); }

    // 함수 실행
    ReturnType Execute(ParamTypes... InArgs) const
    {
        return Func(std::forward<ParamTypes>(InArgs)...);
    }

    // 바인딩된 경우에만 함수 실행
    bool ExecuteIfBound(ParamTypes... InArgs) const
    {
        if (IsBound())
        {
            Execute(std::forward<ParamTypes>(InArgs)...);
            return true;
        }
        return false;
    }
    
    // 복제 함수 - 함수 포인터를 활용한 방식
    TDelegate Duplicate(const TMap<UObject*, UObject*>& DuplicatedObjects) const
    {
        TDelegate Result;
        
        if (!IsBound())
        {
            return Result;
        }
        
        switch (Type)
        {
        case EDelegateType::MemberFunction:
            {
                // 복제된 객체 찾기
                UObject* const* DuplicatedObjectPtr = DuplicatedObjects.Find(BoundObject);
                
                // 복제된 객체가 존재하고 함수 포인터가 유효하면
                if (DuplicatedObjectPtr && *DuplicatedObjectPtr && FunctionPointer && FunctionPointer->IsValid())
                {
                    UObject* DuplicatedObject = *DuplicatedObjectPtr;
                    
                    // 복제된 객체의 타입이 원본 객체의 타입과 호환되는지 확인
                    if (FunctionPointer->GetClassTypeIndex() == std::type_index(typeid(*DuplicatedObject)))
                    {
                        // 클래스 타입과 함수 포인터 타입 가져오기
                        const auto& ObjectType = FunctionPointer->GetClassTypeIndex();
                        
                        // 클래스별 처리 - 각 클래스 타입에 맞는 함수 포인터 캐스팅 필요
                        // 이 부분은 실제 구현에서 리플렉션이나 타입 레지스트리로 구현해야 함
                        DuplicateForObjectType(ObjectType, DuplicatedObject, Result);
                    }
                }
            }
            break;
        case EDelegateType::StaticFunction:
        case EDelegateType::LambdaFunction:
            // 정적 함수와 람다는 그대로 복사
            Result.Func = Func;
            Result.Type = Type;
            break;
        default: 
            break;
        }
        
        return Result;
    }
    
private:
    // 특정 객체 타입에 대한 복제 처리 (템플릿 특수화를 통해 구현)
    void DuplicateForObjectType(const std::type_index& ObjectType, UObject* DuplicatedObject, TDelegate& OutDelegate) const
    {
        // 주의: 이 함수는 실제 구현에서는 리플렉션이나 타입 레지스트리를 활용해야 함
        // 여기서는 개념적으로만 구현
        
        // 예: 알려진 UObject 서브클래스들에 대해 동적으로 캐스팅하여 처리
        if (ObjectType == std::type_index(typeid(UObject)))
        {
            DuplicateForClass<UObject>(DuplicatedObject, OutDelegate);
        }
        // 다른 알려진 클래스 타입들에 대해서도 처리
        // else if (ObjectType == std::type_index(typeid(AActor)))
        // {
        //     DuplicateForClass<AActor>(DuplicatedObject, OutDelegate);
        // }
        // ...등등
    }
    
    // 특정 클래스에 대한 복제 처리
    template <typename T>
    void DuplicateForClass(UObject* DuplicatedObject, TDelegate& OutDelegate) const
    {
        // T 타입으로 함수 포인터 캐스팅
        auto CastedFuncPtr = static_cast<TMemberFunctionPointer<T, ReturnType, ParamTypes...>*>(FunctionPointer.get());
        if (CastedFuncPtr)
        {
            // 함수 포인터 가져오기
            auto MemberFunction = CastedFuncPtr->GetFunctionPtr();
            
            // T 타입으로 객체 캐스팅 및 바인딩
            T* TypedObject = static_cast<T*>(DuplicatedObject);
            if (TypedObject)
            {
                OutDelegate.BindUObject(TypedObject, MemberFunction);
            }
        }
    }
};

template <>
struct std::hash<FDelegateHandle>
{
    size_t operator()(const FDelegateHandle& InHandle) const noexcept
    {
        return std::hash<uint64>()(InHandle.HandleId);
    }
};

// 멀티캐스트 델리게이트 클래스
template <typename Signature>
class TMulticastDelegate;

template <typename ReturnType, typename... ParamTypes>
class TMulticastDelegate<ReturnType(ParamTypes...)>
{
public:
    using FuncType = std::function<ReturnType(ParamTypes...)>;
    
private:
    // 델리게이트 정보 구조체
    struct FDelegateInfo
    {
        FuncType Func;
        UObject* BoundObject = nullptr;
        EDelegateType Type = EDelegateType::None;
        std::shared_ptr<IFunctionPointerBase> FunctionPointer;
        
        FDelegateInfo() = default;
        
        FDelegateInfo(EDelegateType InType, FuncType InFunc, UObject* InObject = nullptr, 
                      std::shared_ptr<IFunctionPointerBase> InFuncPtr = nullptr)
            : Type(InType), Func(std::move(InFunc)), BoundObject(InObject), FunctionPointer(InFuncPtr) {}
    };
    
    // 델리게이트 핸들-정보 맵
    TMap<FDelegateHandle, FDelegateInfo> DelegateHandles;
    
public:
    // UObject에 멤버 함수 추가
    template <typename T, typename FuncPtrType>
        requires std::derived_from<T, UObject>
    FDelegateHandle AddUObject(T* Object, FuncPtrType MemberFunction)
    {
        if (!Object || Object->GetName() == "Destroyed")
        {
            return FDelegateHandle::CreateInvalidHandle();
        }

        // 델리게이트 핸들 생성
        FDelegateHandle DelegateHandle = FDelegateHandle::CreateHandle();
        
        // 함수 포인터 저장
        auto FuncPtr = std::make_shared<TMemberFunctionPointer<T, ReturnType, ParamTypes...>>(MemberFunction);
        
        // 함수 래퍼 생성
        auto AutoForwarder = [ObjectPtr = Object, Function = MemberFunction](ParamTypes... Params)
        {
            if (ObjectPtr && ObjectPtr->GetName() != "Destroyed")
            {
                (ObjectPtr->*Function)(std::forward<ParamTypes>(Params)...);
            }
        };
        
        // 맵에 추가
        DelegateHandles.Add(DelegateHandle, FDelegateInfo(
            EDelegateType::MemberFunction, 
            std::move(AutoForwarder), 
            Object,
            FuncPtr
        ));
        
        return DelegateHandle;
    }
    
    // 정적 함수 추가
    template <typename FunctorType>
    FDelegateHandle AddStatic(FunctorType&& Functor)
    {
        FDelegateHandle DelegateHandle = FDelegateHandle::CreateHandle();
        
        auto AutoForwarder = [Func = std::forward<FunctorType>(Functor)](ParamTypes... Params)
        {
            Func(std::forward<ParamTypes>(Params)...);
        };
        
        DelegateHandles.Add(DelegateHandle, FDelegateInfo(
            EDelegateType::StaticFunction, 
            std::move(AutoForwarder)
        ));
        
        return DelegateHandle;
    }
    
    // 람다 함수 추가
    template <typename FunctorType, typename... Args>
    FDelegateHandle AddLambda(FunctorType&& InFunctor, Args&&... InArgs)
    {
        FDelegateHandle DelegateHandle = FDelegateHandle::CreateHandle();
        
        auto BoundFunc = std::bind(
            std::forward<FunctorType>(InFunctor),
            std::forward<Args>(InArgs)...
        );

        auto LambdaFunc = [BoundFunc](ParamTypes... Params) mutable
        {
            BoundFunc(std::forward<ParamTypes>(Params)...);
        };
        
        DelegateHandles.Add(DelegateHandle, FDelegateInfo(
            EDelegateType::LambdaFunction, 
            std::move(LambdaFunc)
        ));
        
        return DelegateHandle;
    }

    template <typename FunctorType>
    FDelegateHandle AddLambda(FunctorType&& InFunctor)
    {
        FDelegateHandle DelegateHandle = FDelegateHandle::CreateHandle();

        DelegateHandles.Add(
            DelegateHandle, FDelegateInfo(
            EDelegateType::LambdaFunction,
            [Func = std::forward<FunctorType>(InFunctor)](ParamTypes... Params) mutable
            {
                Func(std::forward<ParamTypes>(Params)...);
            }
        ));
        return DelegateHandle;
    }

    // 핸들로 삭제
    bool Remove(FDelegateHandle Handle)
    {
        if (Handle.IsValid())
        {
            DelegateHandles.Remove(Handle);
            return true;
        }
        return false;
    }
    
    // 객체에 바인딩된 모든 함수 삭제
    void RemoveAll(UObject* Object)
    {
        if (!Object) return;
        
        TArray<FDelegateHandle> HandleToRemove;
        
        for (const auto& [Handle, Info] : DelegateHandles)
        {
            if (Info.BoundObject == Object)
            {
                HandleToRemove.Add(Handle);
            }
        }
        
        for (const auto& Handle : HandleToRemove)
        {
            DelegateHandles.Remove(Handle);
        }
    }

    void Clear()
    {
        DelegateHandles.Empty();
    }

    // 브로드캐스트 - 모든 함수 호출
    void Broadcast(ParamTypes... Params) const
    {
        // 유효하지 않은 객체에 바인딩된 델리게이트 제거
        TArray<FDelegateHandle> HandlesToRemove;
        
        for (const auto& [Handle, Info] : DelegateHandles)
        {
            if (Info.BoundObject && (Info.BoundObject->GetName() == "Destroyed"))
            {
                HandlesToRemove.Add(Handle);
            }
        }
        
        for (const auto& Handle : HandlesToRemove)
        {
            const_cast<TMulticastDelegate*>(this)->DelegateHandles.Remove(Handle);
        }
        
        // 남은 델리게이트 실행
        auto CopyDelegates = DelegateHandles;
        for (const auto& [Handle, Info] : CopyDelegates)
        {
            Info.Func(std::forward<ParamTypes>(Params)...);
        }
    }
    
    // 멀티캐스트 델리게이트 복제
    TMulticastDelegate Duplicate(const TMap<UObject*, UObject*>& DuplicatedObjects) const
    {
        TMulticastDelegate Result;
        
        // 모든 델리게이트 순회
        for (const auto& [Handle, Info] : DelegateHandles)
        {
            switch (Info.Type)
            {
            case EDelegateType::MemberFunction:
                {
                    // 복제된 객체 찾기
                    UObject* const* DuplicatedObjectPtr = DuplicatedObjects.Find(Info.BoundObject);
                    
                    // 복제된 객체가 존재하고 함수 포인터가 유효하면
                    if (DuplicatedObjectPtr && *DuplicatedObjectPtr && 
                        Info.FunctionPointer && Info.FunctionPointer->IsValid())
                    {
                        UObject* DuplicatedObject = *DuplicatedObjectPtr;
                        
                        // 복제된 객체의 타입이 원본 객체의 타입과 호환되는지 확인
                        if (Info.FunctionPointer->GetClassTypeIndex() == std::type_index(typeid(*DuplicatedObject)))
                        {
                            // 클래스별 처리
                            DuplicateMemberFunctionForType(
                                Info.FunctionPointer->GetClassTypeIndex(), 
                                DuplicatedObject, 
                                Info.FunctionPointer, 
                                Result);
                        }
                    }
                }
                break;
            case EDelegateType::StaticFunction:
            case EDelegateType::LambdaFunction:
                // 정적 함수와 람다는 그대로 복사
                {
                    FDelegateHandle NewHandle = FDelegateHandle::CreateHandle();
                    Result.DelegateHandles.Add(NewHandle, Info);
                }
                break;
            default: 
                break;
            }
        }
        
        return Result;
    }
    
private:
    // 특정 객체 타입에 대한 델리게이트 복제 처리
    void DuplicateMemberFunctionForType(
        const std::type_index& ObjectType, 
        UObject* DuplicatedObject, 
        std::shared_ptr<IFunctionPointerBase> FuncPtr, 
        TMulticastDelegate& OutDelegate) const;
    
    // 특정 클래스에 대한 델리게이트 복제 처리
    template <typename T>
    void DuplicateMemberFunctionForClass(
        UObject* DuplicatedObject, 
        std::shared_ptr<IFunctionPointerBase> FuncPtr, 
        TMulticastDelegate& OutDelegate) const;
};