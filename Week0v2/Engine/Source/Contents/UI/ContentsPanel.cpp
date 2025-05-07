#include "ContentsPanel.h"

void FContentsPanel::OnResize(HWND hWnd)
{
    RECT clientRect;
    GetClientRect(hWnd, &clientRect);
    Width = clientRect.right - clientRect.left;
    Height = clientRect.bottom - clientRect.top;
}

void FContentsPanel::SetWorld(UWorld* InWorld)
{
    World = InWorld;
}
