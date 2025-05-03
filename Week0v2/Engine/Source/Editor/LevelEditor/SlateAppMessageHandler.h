#pragma once
#include "RawInput.h"
#include "Delegates/DelegateCombination.h"
#include "HAL/PlatformType.h"
#include "InputCore/InputCoreTypes.h"
#include "Math/Vector.h"
#include "SlateCore/Input/Events.h"

namespace EMouseButtons
{
enum Type : uint8;
}

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnKeyCharDelegate, const TCHAR /*Character*/, const bool /*IsRepeat*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnKeyDownDelegate, const FKeyEvent& /*InKeyEvent*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnKeyUpDelegate, const FKeyEvent& /*InKeyEvent*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMouseDownDelegate, const FPointerEvent& /*InMouseEvent*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMouseUpDelegate, const FPointerEvent& /*InMouseEvent*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMouseDoubleClickDelegate, const FPointerEvent& /*InMouseEvent*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMouseWheelDelegate, const FPointerEvent& /*InMouseEvent*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMouseMoveDelegate, const FPointerEvent& /*InMouseEvent*/);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnRawMouseInputDelegate, const FPointerEvent& /*InRawMouseEvent*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnRawKeyboardInputDelegate, const FKeyEvent& /*InRawKeyboardEvent*/);


class FSlateAppMessageHandler
{
public:
    FSlateAppMessageHandler();

    void ProcessMessage(HWND hWnd, uint32 Msg, WPARAM wParam, LPARAM lParam);

public:
    /** Cursor와 관련된 변수를 업데이트 합니다. */
    void UpdateCursorPosition(const FVector2D& NewPos);

    /** 현재 마우스 포인터의 위치를 가져옵니다. */
    FVector2D GetCursorPos() const;

    /** 한 프레임 전의 마우스 포인터의 위치를 가져옵니다. */
    FVector2D GetLastCursorPos() const;

    /** ModifierKeys의 상태를 가져옵니다. */
    FModifierKeysState GetModifierKeys() const;

protected:
    void OnKeyChar(const TCHAR Character, const bool IsRepeat);
    void OnKeyDown(uint32 KeyCode, const uint32 CharacterCode, const bool IsRepeat);
    void OnKeyUp(uint32 KeyCode, const uint32 CharacterCode, const bool IsRepeat);
    void OnMouseDown(const EMouseButtons::Type Button, const FVector2D CursorPos);
    void OnMouseUp(const EMouseButtons::Type Button, const FVector2D CursorPos);
    void OnMouseDoubleClick(const EMouseButtons::Type Button, const FVector2D CursorPos);
    void OnMouseWheel(const float Delta, const FVector2D CursorPos);
    void OnMouseMove();

    void OnRawMouseInput(const RAWMOUSE& RawMouseInput);
    void OnRawKeyboardInput(const RAWKEYBOARD& RawKeyboardInput);
    // 추가적인 함수는 UnrealEngine [SlateApplication.h:1628]을 참조

public:
    FOnKeyCharDelegate OnKeyCharDelegate;
    FOnKeyDownDelegate OnKeyDownDelegate;
    FOnKeyUpDelegate OnKeyUpDelegate;
    FOnMouseDownDelegate OnMouseDownDelegate;
    FOnMouseUpDelegate OnMouseUpDelegate;
    FOnMouseDoubleClickDelegate OnMouseDoubleClickDelegate;
    FOnMouseWheelDelegate OnMouseWheelDelegate;
    FOnMouseMoveDelegate OnMouseMoveDelegate;

    FOnRawMouseInputDelegate OnRawMouseInputDelegate;
    FOnRawKeyboardInputDelegate OnRawKeyboardInputDelegate;

private:
    struct EModifierKey
    {
        enum Type : uint8
        {
            LeftShift,    // VK_LSHIFT
            RightShift,   // VK_RSHIFT
            LeftControl,  // VK_LCONTROL
            RightControl, // VK_RCONTROL
            LeftAlt,      // VK_LMENU
            RightAlt,     // VK_RMENU
            LeftWin,      // VK_LWIN
            RightWin,     // VK_RWIN
            CapsLock,     // VK_CAPITAL
            Count,
        };
    };

    // Cursor Position
    FVector2D CurrentPosition;
    FVector2D PreviousPosition;

    bool ModifierKeyState[EModifierKey::Count];
    TSet<EKeys::Type> PressedMouseButtons;

    std::unique_ptr<FRawInput> RawInputHandler;

private:
    void HandleRawInput(const RAWINPUT& RawInput);
};
