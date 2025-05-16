#pragma once
#include <memory>

#include "Animation/CustomAnimInstance/TestAnimInstance.h"
#include "Container/Array.h"
#include "Container/String.h"
#include "Delegates/DelegateCombination.h"
#include "HAL/PlatformType.h"

class ASkeletalMeshActor;
class APlayerCameraManager;

enum class EGameState : uint8
{
    None,
    PrepareRestart,
    Playing,
    //Paused,
    PrepareEnded,
    Ended
};

enum class EEventType : uint8
{
    None,
    Single,
    Duo,
    Squad,
    Max
};

class FGameManager 
{
    DECLARE_MULTICAST_DELEGATE(GameOverEventDelegate)
public:
    static FGameManager& Get();
    
    float GetGameTimer() const { return GameTimer; }
    void SetGameTimer(float NewTime) { GameTimer = NewTime; }
    void ResetGameTimer() { GameTimer = 0.0f; }
    void UpdateGameTimer(float DeltaTime) { GameTimer += DeltaTime; }

    void BeginPlay();
    void RestartGame();
    void StartGame();
    static void EndGame();

    static int GetScore() { return Score; }
    static void SetScore(int NewScore) { Score = NewScore; }
    static void AddScore() { Score++; printf("Get Score");}
    
    void SetGameState(EGameState NewState) { CurrentGameState = NewState; }
    void Tick(float DeltaTime);
    void EditorTick(float DeltaTime);
    EGameState GetGameState() const { return CurrentGameState; }
    
    void StartNPCAnimation(int index, ETestState AnimState);
    void StartTrack(float Duration, ETestState AnimState, EEventType EventCase);

    static int GetHealth() { return Health; }
    static void LoseHealth();
    
    static void PlayAnimA();
    static void PlayAnimB();
    static void PlayAnimC();
    
    TArray<ASkeletalMeshActor*> NPCs;
    inline static GameOverEventDelegate GameOverEvent;
private:
    // 싱글톤을 위한 정적 인스턴스
    static std::shared_ptr<FGameManager> Instance;

    inline static EGameState CurrentGameState = EGameState::None;

    
    inline static int Score = 0;
    inline static int Health = 3;
    inline static float GameTimer = 0.0f;
    inline static float MinSuccessTime = 0.0f;
    inline static float MaxSuccessTime = 0.0f;
    float AnimTime[4] = { 0.0f };
    inline static ETestState CurrentAnimState = ETestState::None;
    inline static bool bGetTrackScore = true; 
    inline static bool bGameOver = false;
    bool bTrackProcess = false;

    
};




