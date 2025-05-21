#pragma once
#include <string>

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "Math/NumericLimits.h"

struct FVector;
struct FRotator;

struct FControlInfo
{
    std::string Label;
    ImVec4 ButtonColor;
    ImVec4 ButtonHoveredColor;
    ImVec4 ButtonActiveColor;
    float* ValuePtr; // 해당 컴포넌트에 대한 포인터
};

struct FImGuiWidget
{
    static bool DrawVec3Control(const std::string& Label, FVector& Values, float ResetValue = 0.0f, float ColumnWidth = 100.0f);
    static bool DrawRot3Control(const std::string& Label, FRotator& Values, float ResetValue = 0.0f, float ColumnWidth = 100.0f);
    static void DrawDragInt(const std::string& label, int& value, int min = 0, int max = 0, float width = 100.0f);
    static void DrawDragFloat(const std::string& label, float& value, float min = 0.0f, float max = 0.0f, float width = 100.0f);

    template <size_t N>
    static bool DisplayNControl(
        const std::string& Label,
        float ResetValue,
        float ColumnWidth,
        const char* Format,
        const std::array<FControlInfo, N>& ComponentsInfo
    )
    {
        bool bChanged = false;

        const ImGuiIO& IO = ImGui::GetIO();
        ImFont* BoldFont = IO.Fonts->Fonts[0];

        ImGui::PushID(Label.c_str());

        // 가운데 정렬 로직
        const float WindowWidth = ImGui::GetWindowWidth();

        // N개의 DragFloat와 버튼을 고려한 ControlWidth 계산
        const float SingleItemWidth = 5.0f + ImGui::CalcItemWidth() + 5.0f; // 버튼(5) + DragFloat + 간격(5)
        const float TotalItemsWidth = static_cast<float>(N) * SingleItemWidth;
        const float ControlWidth = ColumnWidth + TotalItemsWidth + ImGui::GetStyle().ItemSpacing.x * (static_cast<float>(N) - 1.0f);
        const float Offset = (WindowWidth - ControlWidth) * 0.5f;
        if (Offset > 0)
        {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + Offset);
        }

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, ColumnWidth);
        ImGui::Text("%s", Label.c_str());
        ImGui::NextColumn();

        ImGui::PushMultiItemsWidths(static_cast<int>(N), ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);

        const float LineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        const ImVec2 ButtonSize = {
            // BoldFont->CalcTextSizeA(BoldFont->FontSize, FLT_MAX, 0, "III").x + GImGui->Style.FramePadding.x * 2.0f,
            5,
            LineHeight
        };

        for (size_t Idx = 0; Idx < N; ++Idx)
        {
            const FControlInfo& Info = ComponentsInfo[Idx];
            ImGui::PushID(static_cast<int>(Idx)); // 각 컴포넌트 위젯에 대한 고유 ID

            // Button
            ImGui::PushStyleColor(ImGuiCol_Button, Info.ButtonColor);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Info.ButtonHoveredColor);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, Info.ButtonActiveColor);
            ImGui::PushFont(BoldFont);
            if (ImGui::Button(("##" + Info.Label + "Btn").c_str(), ButtonSize))
            {
                *Info.ValuePtr = ResetValue;
            }
            ImGui::PopFont();
            ImGui::PopStyleColor(3);

            // Drag
            constexpr float Min = TNumericLimits<float>::Lowest();
            constexpr float Max = TNumericLimits<float>::Max();

            ImGui::SameLine();
            if (ImGui::DragScalar(
                ("##" + Info.Label + "Drag").c_str(),
                ImGuiDataType_Float,
                Info.ValuePtr,
                0.1f,
                &Min, &Max, Format
            ))
            {
                bChanged = true;
            }

            if (Idx < N - 1)
            {
                ImGui::SameLine(0, 5); // 마지막 컴포넌트 뒤에는 간격 X
            }

            ImGui::PopItemWidth();
            ImGui::PopID();
        }

        ImGui::PopStyleVar(2); // ItemSpacing, FrameRounding
        ImGui::Columns(1);
        ImGui::PopID();

        return bChanged;
    }
};
