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
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 displaySize = io.DisplaySize; // 뷰포트 크기
    // if (!GameMgr.IsGameActive()) {
    //     return;
    // }


    // --- ImGui 창 설정 ---
    // 현재 게임 상태 확인
    EGameState currentState = GameMgr.GetGameState(); // GetGameState() 함수가 있다고 가정

    // --- 게임 오버 상태 UI 렌더링 ---
    if (currentState == EGameState::Ended)
    {
        // --- 반투명 배경 및 입력 차단 창 설정 ---
        // 화면 전체를 덮는 반투명 창 생성 (기존 게임 화면을 어둡게)
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0)); // 패딩 제거
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f)); // 검정색 반투명 배경 (알파값 0.7)

        ImGuiWindowFlags gameOverFlags =
            ImGuiWindowFlags_NoDecoration | // 제목 표시줄 없음
            ImGuiWindowFlags_NoMove |       // 이동 불가
            ImGuiWindowFlags_NoResize |     // 크기 조절 불가
            ImGuiWindowFlags_NoScrollbar |  // 스크롤바 없음
            ImGuiWindowFlags_NoCollapse |   // 접기 불가
            ImGuiWindowFlags_NoSavedSettings; // 설정 저장 안 함
            // ImGuiWindowFlags_NoInputs 제거: 버튼 클릭을 위해 입력 필요
            // ImGuiWindowFlags_NoBackground 제거: 반투명 배경 적용 위해

        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(displaySize);

        if (ImGui::Begin("GameOverScreen", nullptr, gameOverFlags))
        {
            // --- UI 요소 중앙 정렬 및 렌더링 ---
            float windowWidth = ImGui::GetWindowSize().x;
            float windowHeight = ImGui::GetWindowSize().y;

            // 1. "GAME OVER" 텍스트
            const char* gameOverText = "GAME OVER";
            ImGui::SetWindowFontScale(2.5f); // 큰 폰트
            ImVec2 gameOverTextSize = ImGui::CalcTextSize(gameOverText);
            ImGui::SetCursorPosX((windowWidth - gameOverTextSize.x) * 0.5f);
            ImGui::SetCursorPosY(windowHeight * 0.3f); // 화면 위쪽 30% 지점 정도
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.1f, 0.1f, 1.0f)); // 빨간색
            ImGui::TextUnformatted(gameOverText);
            ImGui::PopStyleColor();
            ImGui::SetWindowFontScale(1.0f); // 폰트 크기 복원

            // 2. 최종 점수 텍스트
            int finalScore = GameMgr.GetScore(); // 최종 점수 가져오기
            char scoreTextBuffer[64];
            sprintf_s(scoreTextBuffer, "Final Score: %d", finalScore);
            ImGui::SetWindowFontScale(1.8f); // 약간 큰 폰트
            ImVec2 scoreTextSize = ImGui::CalcTextSize(scoreTextBuffer);
            ImGui::SetCursorPosX((windowWidth - scoreTextSize.x) * 0.5f);
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + gameOverTextSize.y + 30); // "GAME OVER" 아래 간격
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f)); // 노란색
            ImGui::TextUnformatted(scoreTextBuffer);
            ImGui::PopStyleColor();
            ImGui::SetWindowFontScale(1.0f);

            // 3. 재시작 버튼
            const char* restartButtonText = "Restart Game";
            ImGui::SetWindowFontScale(1.5f);
            ImVec2 buttonSize = ImVec2(ImGui::CalcTextSize(restartButtonText).x + 40, 50); // 버튼 크기 (텍스트 + 패딩)
            ImGui::SetCursorPosX((windowWidth - buttonSize.x) * 0.5f);
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + scoreTextSize.y + 50); // 점수 아래 간격
            if (ImGui::Button(restartButtonText, buttonSize))
            {
                // --- 재시작 로직 호출 ---
                GameMgr.RestartGame(); // GameManager에 재시작 함수 호출 (RestartGame() 함수가 있다고 가정)
            }
            ImGui::SetWindowFontScale(1.0f);

            ImGui::End();
        }
        ImGui::PopStyleColor(); // WindowBg
        ImGui::PopStyleVar(); // WindowPadding
    }
    // --- 일반 게임 HUD 렌더링 (게임이 종료되지 않았을 때) ---
    else  // 또는 다른 활성 상태
    {
        // 기존의 점수, 시간, 크로스헤어 등을 그리는 코드
        ImGuiWindowFlags hud_flags =
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoInputs; // HUD는 입력 무시

        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(displaySize);

        if (ImGui::Begin("GameHUD", nullptr, hud_flags))
        {
            // 1. 점수 표시
            int score = GameMgr.GetScore();
            ImGui::SetCursorPos(ImVec2(250, 30));
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
            ImGui::SetWindowFontScale(1.5f);
            char scoreText[64];
            sprintf_s(scoreText, "Score: %d", score);
            ImGui::Text(scoreText);
            ImGui::SetWindowFontScale(1.0f);
            ImGui::PopStyleColor();

            // 2. 생명 표시
            int Health = GameMgr.GetHealth();
            ImGui::SetCursorPos(ImVec2(250, 60));
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
            ImGui::SetWindowFontScale(1.5f);
            char HealthText[64];
            sprintf_s(HealthText, "Health: %d", Health);
            ImGui::Text(HealthText);
            ImGui::SetWindowFontScale(1.0f);
            ImGui::PopStyleColor();

            // 3. 크로스헤어 (기존 코드와 동일)
            // ImDrawList* drawList = ImGui::GetWindowDrawList();
            // ... (크로스헤어 그리는 로직) ...

            ImGui::End();
        }
    }
}

void FGameUIPanel::OnResize(HWND hWnd)
{
}
