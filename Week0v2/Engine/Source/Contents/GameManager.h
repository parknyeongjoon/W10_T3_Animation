#pragma once
#include <memory>

class FGameManager 
{
    
public:
    static FGameManager& Get();

public:
    float GetGameTimer() const { return GameTimer; }
    void SetGameTimer(float NewTime) { GameTimer = NewTime; }
    void ResetGameTimer() { GameTimer = 0.0f; }

    void BeginPlay();
    
    void RestartGame();
    void StartGame();
    void EndGame();


private:
    int Score = 0;
    float GameTimer = 0.0f;
    
    // 싱글톤을 위한 정적 인스턴스
    static std::shared_ptr<FGameManager> Instance;
};
