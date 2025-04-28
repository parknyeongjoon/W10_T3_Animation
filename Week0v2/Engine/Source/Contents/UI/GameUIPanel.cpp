#include "GameUIPanel.h"

#include "Contents/GameManager.h"
#include "ImGUI/imgui.h"

FGameUIPanel::~FGameUIPanel()
{
}

void FGameUIPanel::Render()
{
    // --- 게임 상태 확인 ---
    // 만약 게임 매니저가 없거나 게임이 활성 상태가 아니면 UI를 그리지 않음
    // (FGameManager에 IsGameActive() 같은 함수가 있다고 가정)
    FGameManager& GameMgr = FGameManager::Get(); // 싱글톤 인스턴스 가져오기
    // if (!GameMgr.IsGameActive()) {
    //     return;
    // }


    // --- ImGui 창 설정 ---
    // 화면 전체를 덮는 투명한 창을 사용하여 UI 요소 배치
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 displaySize = io.DisplaySize; // 렌더링 타겟(뷰포트) 크기

    // 창 플래그 설정: 제목 표시줄, 배경, 이동, 크기 조절, 스크롤바 등을 비활성화하고 입력 무시
    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoDecoration |       // 제목 표시줄, 테두리 등 없음
        ImGuiWindowFlags_NoMove |             // 이동 불가
        ImGuiWindowFlags_NoResize |           // 크기 조절 불가
        ImGuiWindowFlags_NoScrollbar |        // 스크롤바 없음
        ImGuiWindowFlags_NoCollapse |         // 접기 불가
        ImGuiWindowFlags_NoBackground |       // 배경 투명
        ImGuiWindowFlags_NoInputs;            // 마우스/키보드 입력 무시 (게임 입력 방해 안 함)

    // 창 위치 및 크기 설정 (화면 전체 사용)
    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(displaySize);

    // --- UI 렌더링 시작 ---
    if (ImGui::Begin("GameHUD", nullptr, window_flags))
    {
        // --- 게임 데이터 가져오기 ---
        int score = GameMgr.GetScore();
        float remainingTime = GameMgr.GetRemainingTime();

        // --- UI 요소 그리기 ---
        // 1. 점수 표시 (예: 화면 좌상단)
        ImGui::SetCursorPos(ImVec2(20, 20)); // 위치 조정 (좌상단 기준)
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f)); // 노란색 텍스트
        ImGui::SetWindowFontScale(1.5f); // 폰트 크기 조절 (필요시)
        char scoreText[64];
        sprintf_s(scoreText, "Score: %d", score);
        ImGui::Text(scoreText);
        ImGui::SetWindowFontScale(1.0f); // 폰트 크기 복원
        ImGui::PopStyleColor();

        // 2. 남은 시간 표시 (예: 화면 우상단)
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); // 흰색 텍스트
        ImGui::SetWindowFontScale(1.5f);
        char timeText[64];
        sprintf_s(timeText, "Time: %.1f", remainingTime > 0.0f ? remainingTime : 0.0f); // 0 이하로 안내려가게
        // 텍스트 길이를 측정하여 우측 정렬
        ImVec2 textSize = ImGui::CalcTextSize(timeText);
        ImGui::SetCursorPos(ImVec2(displaySize.x - textSize.x - 20, 20)); // 위치 조정 (우상단 기준)
        ImGui::Text(timeText);
        ImGui::SetWindowFontScale(1.0f);
        ImGui::PopStyleColor();

        // 3. 크로스헤어 (예: 화면 중앙) - 간단한 '+' 모양
        // ImDrawList* drawList = ImGui::GetWindowDrawList();
        // float crosshairSize = 10.0f; // 크로스헤어 반 크기
        // ImVec2 center = ImVec2(displaySize.x * 0.5f, displaySize.y * 0.5f);
        // ImU32 crosshairColor = IM_COL32(255, 255, 255, 200); // 흰색 (약간 투명)
        // float thickness = 1.5f;
        //
        // drawList->AddLine(
        //     ImVec2(center.x - crosshairSize, center.y),
        //     ImVec2(center.x + crosshairSize, center.y),
        //     crosshairColor,
        //     thickness
        // );
        // drawList->AddLine(
        //     ImVec2(center.x, center.y - crosshairSize),
        //     ImVec2(center.x, center.y + crosshairSize),
        //     crosshairColor,
        //     thickness
        // );

        // (필요시) 추가적인 UI 요소 (예: 게임 오버 메시지, 재시작 버튼 등)
        // if (remainingTime <= 0.0f) {
        //     ImGui::SetCursorPos(ImVec2(center.x - 100, center.y - 50)); // 중앙 근처
        //     ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // 빨간색
        //     ImGui::SetWindowFontScale(2.0f);
        //     ImGui::Text("GAME OVER");
        //     ImGui::SetWindowFontScale(1.0f);
        //     ImGui::PopStyleColor();
        //     // 재시작 버튼 등은 NoInputs 플래그 때문에 직접 상호작용 불가.
        //     // 별도의 입력 처리나 다른 ImGui 창 필요.
        // }

        
        ImGui::End();
    }
}

void FGameUIPanel::OnResize(HWND hWnd)
{
}
