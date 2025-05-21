#include "PrimitiveDrawEditor.h"

void PrimitiveDrawEditor::Render()
{
    for (auto Rect :DrawRectInfo)
    {
        ImGui::GetForegroundDrawList()->AddRect(
            Rect.topLeft,
            Rect.bottomRight,
            Rect.color,
            Rect.rounding,
            Rect.drawFlags,
            Rect.thickness);
    }
    ClearRenderInfo();
}

void PrimitiveDrawEditor::OnResize(HWND hWnd)
{
    
}

void PrimitiveDrawEditor::ClearRenderInfo()
{
    DrawRectInfo.Empty();
}
