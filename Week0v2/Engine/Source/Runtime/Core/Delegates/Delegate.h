#pragma once
#include <functional>
#include "Core/Container/Map.h"
#include "UObject/Object.h"

#define FUNC_DECLARE_DELEGATE(DelegateName, ReturnType, ...) \
	using DelegateName = TDelegate<ReturnType(__VA_ARGS__)>;

#define FUNC_DECLARE_MULTICAST_DELEGATE(MulticastDelegateName, ReturnType, ...) \
	using MulticastDelegateName = TMulticastDelegate<ReturnType(__VA_ARGS__)>;


class UObject;

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

		// Overflow가 발생했다면
		if (Result == 0)
		{
			// 한번 더 더하기
			Result = NextHandleId.fetch_add(1, std::memory_order_relaxed);
		}

		return Result;
	}

public:
	static FDelegateHandle CreateHandle()
	{
		return FDelegateHandle{GenerateNewID()};
	}

    static FDelegateHandle CreateInvalidHandle()
	{
	    return FDelegateHandle{0};
	}

	bool IsValid() const { return HandleId != 0; }
	void Invalidate() { HandleId = 0; }

	bool operator==(const FDelegateHandle& Other) const
	{
		return HandleId == Other.HandleId;
	}

	bool operator!=(const FDelegateHandle& Other) const
	{
		return HandleId != Other.HandleId;
	}
};

template <typename Signiture>
class TDelegate;

template <typename ReturnType, typename... ParamTypes>
class TDelegate<ReturnType(ParamTypes...)>
{
	using FuncType = std::function<ReturnType(ParamTypes...)>;
	FuncType Func;

public:
    template <typename T, typename FunctorType>
        requires std::derived_from<T, UObject>
    void BindUObject(T* Object, FunctorType&& Functor)
	{
	    if (!Object || Object->GetName() == "Destroyed")
	    {
	        UnBind();
	        return;
	    }

        void* ObjectPtr = Object;
        
        Func = [ObjectPtr, Function = std::forward<FunctorType>(Functor)](ParamTypes... Params) -> ReturnType
        {
            T* CurrentObject = static_cast<T*>(ObjectPtr);
            
            if (CurrentObject && CurrentObject->GetName() != "Destroyed")
            {
                return (CurrentObject->*Function)(std::forward<ParamTypes>(Params)...);
            }

            return ReturnType();
        };
	}

    template <typename FunctorType>
    void BindStatic(FunctorType&& Functor)
    {
        Func = [StaticFunction = std::forward<FunctorType>(Functor)](ParamTypes... Params) -> ReturnType
        {
            // 함수 호출 시 매개변수 전달
            return StaticFunction(std::forward<ParamTypes>(Params)...);
        };
    }
    
	template <typename FunctorType>
	void BindLambda(FunctorType&& InFunctor)
	{
	    Func = [Func = std::forward<FunctorType>(InFunctor)](ParamTypes... Params)
	    {
	        return Func(std::forward<ParamTypes>(Params)...);
	    };
	}

	void UnBind()
	{
		Func = nullptr;
	}

	bool IsBound() const
	{
	    return static_cast<bool>(Func);
	}

	ReturnType Execute(ParamTypes... InArgs) const
	{
		return Func(std::forward<ParamTypes>(InArgs)...);
	}

	bool ExecuteIfBound(ParamTypes... InArgs) const
	{
		if (IsBound())
		{
			Execute(std::forward<ParamTypes>(InArgs)...);
			return true;
		}
		return false;
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

template <typename Signature>
class TMulticastDelegate;

template <typename ReturnType, typename... ParamTypes>
class TMulticastDelegate<ReturnType(ParamTypes...)>
{
    using FuncType = std::function<ReturnType(ParamTypes...)>;
    
    // 델리게이트 정보를 저장하는 구조체
    struct FDelegateInfo
    {
        FuncType Func;
        UObject* BoundObject;  // 바인딩된 UObject를 저장 (없으면 nullptr)
        
        FDelegateInfo() : BoundObject(nullptr) {}
        FDelegateInfo(FuncType InFunc, UObject* InObject = nullptr) 
            : Func(std::move(InFunc)), BoundObject(InObject) {}
    };
    
    TMap<FDelegateHandle, FDelegateInfo> DelegateHandles;
    
public:
    template <class T, typename FunctorType>
        requires std::derived_from<T, UObject>
    FDelegateHandle AddUObject(T* Object, FunctorType&& Functor)
    {
        if (!Object || Object->GetName() == "Destroyed")
        {
            return FDelegateHandle::CreateInvalidHandle();  // 유효하지 않은 핸들 반환
        }

        FDelegateHandle DelegateHandle = FDelegateHandle::CreateHandle();
    
        auto AutoForwarder = [ObjectPtr = Object, Func = std::forward<FunctorType>(Functor)](ParamTypes... Params)
        {
            if (ObjectPtr && ObjectPtr->GetName() != "Destroyed")
            {
                (ObjectPtr->*Func)(std::forward<ParamTypes>(Params)...);
            }
        };
    
        // 함수와 바인딩된 객체에 대한 정보를 함께 저장
        DelegateHandles.Add(DelegateHandle, FDelegateInfo(std::move(AutoForwarder), Object));
    
        return DelegateHandle;
    }
    
    // 다른 Add* 메서드들도 유사하게 수정 (Static과 Lambda는 바인딩된 객체가 없음)
    template <typename FunctorType>
    FDelegateHandle AddStatic(FunctorType&& Functor)
    {
        FDelegateHandle DelegateHandle = FDelegateHandle::CreateHandle();
    
        auto AutoForwarder = [Func = std::forward<FunctorType>(Functor)](ParamTypes... Params)
        {
            Func(std::forward<ParamTypes>(Params)...);
        };
    
        // 바인딩된 객체 없음
        DelegateHandles.Add(DelegateHandle, FDelegateInfo(std::move(AutoForwarder), nullptr));
    
        return DelegateHandle;
    }
    
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
        
        // 바인딩된 객체 없음
        DelegateHandles.Add(DelegateHandle, FDelegateInfo(std::move(LambdaFunc), nullptr));
        
        return DelegateHandle;
    }

    bool Remove(FDelegateHandle Handle)
    {
        if (Handle.IsValid())
        {
            DelegateHandles.Remove(Handle);
            return true;
        }
        return false;
    }
    
    // 객체 기반으로 등록된 모든 델리게이트 제거
    void RemoveAll(UObject* Object)
    {
        if (!Object) return;
        
        TArray<FDelegateHandle> HandleToRemove;
        
        // 삭제할 핸들 찾기
        for (const auto& [Handle, Info] : DelegateHandles)
        {
            if (Info.BoundObject == Object)
            {
                HandleToRemove.Add(Handle);
            }
        }
        
        // 핸들 제거
        for (const auto& Handle : HandleToRemove)
        {
            DelegateHandles.Remove(Handle);
        }
    }

    void Broadcast(ParamTypes... Params) const
    {
        // 실행 전에 유효하지 않은 객체를 가진 델리게이트를 제거하기 위한 리스트
        TArray<FDelegateHandle> HandlesToRemove;
        
        // 유효하지 않은 객체를 찾기
        for (const auto& [Handle, Info] : DelegateHandles)
        {
            if (Info.BoundObject && (Info.BoundObject == nullptr || Info.BoundObject->GetName() == "Destroyed"))
            {
                HandlesToRemove.Add(Handle);
            }
        }
        
        // 유효하지 않은 객체에 바인딩된 델리게이트 제거
        for (const auto& Handle : HandlesToRemove)
        {
            const_cast<TMulticastDelegate*>(this)->DelegateHandles.Remove(Handle);
        }
        
        // 남은 델리게이트 실행
        auto CopyDelegates = DelegateHandles;
        for (const auto& [Handle, Info] : CopyDelegates)
        {
            // 한 번 더 검사 (안전을 위해)
            if (!Info.BoundObject || (Info.BoundObject && Info.BoundObject->GetName() != "Destroyed"))
            {
                Info.Func(std::forward<ParamTypes>(Params)...);
            }
        }
    }
};
