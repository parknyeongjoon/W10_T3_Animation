#pragma once
#include <memory>

class APlayerCameraManager;

enum class EGameState
{
    None,
    PrepareRestart,
    Playing,
    //Paused,
    PrepareEnded,
    Ended
};

class FGameManager 
{
    
public:
    static FGameManager& Get();

public:
    float GetGameTimer() const { return GameTimer; }
    void SetGameTimer(float NewTime) { GameTimer = NewTime; }
    void ResetGameTimer() { GameTimer = 0.0f; }
    void UpdateGameTimer(float DeltaTime) { GameTimer += DeltaTime / 1000.0f; }

    void BeginPlay();
    
    void RestartGame();
    void StartGame();
    void EndGame();

    int GetScore() const { return Score; }
    void SetScore(int NewScore) { Score = NewScore; }

    static void AddScore() { Get().Score++;}
    static void StaticSpawnEnemy() {}
    static void SpawnEnemy();
    
    void SetGameState(EGameState NewState) { CurrentGameState = NewState; }
    void Tick(float DeltaTime);
    void EditorTick(float DeltaTime);
    EGameState GetGameState() const { return CurrentGameState; }

    float GetRemainingTime() const { return GameOverTimer - GameTimer; }

private:
    int Score = 0;
    float GameTimer = 0.0f;
    float GameOverTimer = 10.0f;
    EGameState CurrentGameState = EGameState::None;

    APlayerCameraManager* PlayerCameraManager = nullptr;
    
    
    // 싱글톤을 위한 정적 인스턴스
    static std::shared_ptr<FGameManager> Instance;
};




