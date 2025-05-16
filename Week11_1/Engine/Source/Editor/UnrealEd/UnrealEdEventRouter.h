#pragma once

#include <functional>
#include <Math/Vector.h>

#include "Container/Array.h"
#include "Container/String.h"

#define FORWARD_EVENT_GENERIC(LOCAL_EVENT, GLOBAL_EVENT) \
LOCAL_EVENT.Subscribe([](auto&&... args) { (GLOBAL_EVENT).Invoke(std::forward<decltype(args)>(args)...); })

template<typename... Args>
class UnrealEdEventRouter {
public:
    // Declare callback types...
    using Callback = std::function<void(Args...)>;

    // Subscribe callback
    void Subscribe(const Callback& callback) {
        subscribers.push_back(callback);
    }

    // raised event...
    void Invoke(Args... args) {
        for (const auto& cb : subscribers) {
            cb(args...);
        }
    }

private:
    TArray<Callback> subscribers;
};


class UEditorEvent
{
public:
    static UEditorEvent& GetInstance()
    {
        static UEditorEvent Instance;
        return Instance;
    }

    UnrealEdEventRouter<const FString&, uint32>     EVENT_ROUTER_UNSIGNED_INTEGER;
    UnrealEdEventRouter<const FString&, int32>      EVENT_ROUTER_INTEGER;
    UnrealEdEventRouter<const FString&, float>      EVENT_ROUTER_FLOAT;
    UnrealEdEventRouter<const FString&, bool>       EVENT_ROUTER_BOOL;
    UnrealEdEventRouter<const FString&, FString>    EVENT_ROUTER_FString;
    UnrealEdEventRouter<const FString&, FWString>   EVENT_ROUTER_FWString;
    UnrealEdEventRouter<const FString&, FVector>    EVENT_ROUTER_FVector;

private:
    UEditorEvent() = default;
    ~UEditorEvent() = default;
    UEditorEvent(const UEditorEvent&) = delete;
    UEditorEvent& operator=(const UEditorEvent&) = delete;
};