#pragma once
#include <functional>
#include <string>

#include "UserInterface/Console.h"


class FRawInput
{
public:
    using InputCallback = std::function<void(const RAWINPUT& RawInput)>;

    FRawInput(HWND hWnd, InputCallback InCallback);

    ~FRawInput()
    {
        UnregisterDevices();
    }

    FRawInput(const FRawInput&) = delete;
    FRawInput& operator=(const FRawInput&) = delete;
    FRawInput(FRawInput&&) = delete;
    FRawInput& operator=(FRawInput&&) = delete;

public:
    // 메시지 처리 함수
    void ProcessRawInput(LPARAM lParam) const;

    void ReRegisterDevices()
    {
        UnregisterDevices();
        RegisterDevices();
    }

    bool IsValid() const;

private:
    void RegisterDevices();
    void UnregisterDevices();

    static std::string GetLastErrorString();
    static void LogRawInput(LogLevel Level, const std::string& Message);

private:
    HWND AppWnd;
    InputCallback Callback;

    bool bIsValid = false;
};
