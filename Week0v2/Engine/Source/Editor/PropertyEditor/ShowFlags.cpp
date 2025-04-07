#include "PropertyEditor/ShowFlags.h"
#include "Engine/World.h"

ShowFlags::ShowFlags()
{
    currentFlags = 31;
}

ShowFlags::~ShowFlags()
{
}

ShowFlags& ShowFlags::GetInstance()
{
	static ShowFlags instance;
	return instance;
}

void ShowFlags::Draw(std::shared_ptr<FEditorViewportClient> ActiveViewport)
{
	float controllWindowWidth = static_cast<float>(width) * 0.12f;
	float controllWindowHeight = static_cast<float>(height) * 0.f;

	float controllWindowPosX = (static_cast<float>(width) - controllWindowWidth) * 0.64f;
	float controllWindowPosY = (static_cast<float>(height) - controllWindowHeight) * 0.f;

	// 창 크기와 위치 설정
	ImGui::SetNextWindowPos(ImVec2(controllWindowPosX, controllWindowPosY));
	ImGui::SetNextWindowSize(ImVec2(controllWindowWidth, controllWindowHeight), ImGuiCond_Always);

	if (ImGui::Begin("ShowFlags"))
	{
		const char* items[] = { "AABB", "Primitves","BillBoardText","UUID", "Fog"};
        uint64 curFlag = ActiveViewport->GetShowFlag();
		bool selected[IM_ARRAYSIZE(items)] = { curFlag & static_cast<uint64>(EEngineShowFlags::SF_AABB),
            curFlag& static_cast<uint64>(EEngineShowFlags::SF_Primitives),
            curFlag& static_cast<uint64>(EEngineShowFlags::SF_BillboardText),
            curFlag& static_cast<uint64>(EEngineShowFlags::SF_UUIDText),
            curFlag& static_cast<uint64>(EEngineShowFlags::SF_Fog)
        };  // 각 항목의 체크 상태 저장

		if (ImGui::BeginCombo("Show Flags", "Select Show Flags"))
		{
			for (int i = 0; i < IM_ARRAYSIZE(items); i++)
			{
				ImGui::Checkbox(items[i], &selected[i]); 
			}
			ImGui::EndCombo(); 
		}
		ActiveViewport->SetShowFlag(ConvertSelectionToFlags(selected));

	}
	ImGui::End(); // 윈도우 종료
}
uint64 ShowFlags::ConvertSelectionToFlags(const bool selected[])
{
	uint64 flags = static_cast<uint64>(EEngineShowFlags::None);

	if (selected[0])
		flags |= static_cast<uint64>(EEngineShowFlags::SF_AABB);
	if (selected[1])
		flags |= static_cast<uint64>(EEngineShowFlags::SF_Primitives);
	if (selected[2])
		flags |= static_cast<uint64>(EEngineShowFlags::SF_BillboardText);
	if (selected[3])
		flags |= static_cast<uint64>(EEngineShowFlags::SF_UUIDText);
    if (selected[4])
        flags |= static_cast<uint64>(EEngineShowFlags::SF_Fog);
	return flags;
}

void ShowFlags::OnResize(HWND hWnd)
{
	RECT clientRect;
	GetClientRect(hWnd, &clientRect);
	width = clientRect.right - clientRect.left;
	height = clientRect.bottom - clientRect.top;
}