#pragma once
#include <functional>

#define DECLARE_CONTEXT_FUNC(FunctorType, ReturnType, ...) \
    using FunctorType = FFunctorWithContext<ThisClass, ReturnType, __VA_ARGS__>; \
    friend FunctorType; \

// 멤버 함수로 구현하지 않고 함수에 대한 friend로 객체에 접근 가능하게 만듬.
// TODO: Delegate(멤버 함수를 통한)로 대체 고려.
// TODO: self 삭제 시 대응.
template <typename ClassType, typename ReturnType, typename... ParamTypes>
struct FFunctorWithContext
{
public:
    using FuncPtrType = std::function<ReturnType(ClassType*, ParamTypes...)>;
public:
    FFunctorWithContext() = default;
    FFunctorWithContext(ClassType* self): self(self) {}
    FFunctorWithContext(ClassType* self, FuncPtrType funcPtr): self(self), func(funcPtr) {}
private:
    ClassType* self;
public:
    FuncPtrType func = nullptr;
    ReturnType operator()() const
    {
        return func(self);
    }
    ReturnType operator()(ParamTypes... params) const
    {
        return func(self, params...);
    }
    void Bind(ClassType* self)
    {
        this->self = self;
    }

    // ClassType 간에 상속 관계면 func도 암시적으로 변환.
    template <typename BaseType, std::enable_if_t<std::is_base_of<BaseType, ClassType>::value, int> = 0>
    operator FFunctorWithContext<BaseType, ReturnType, ParamTypes...>() const
    {
        std::function<ReturnType(BaseType*, ParamTypes...)> convertedFunc = [this](BaseType* Obj, ParamTypes... params)
        {
            return func(static_cast<ClassType*>(Obj), params...);
        };
        FFunctorWithContext<BaseType, ReturnType, ParamTypes...> result(static_cast<BaseType*>(self));
        result.func = convertedFunc;
        return result;
    }
};
