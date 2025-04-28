#include "GameManager.h"

#include "AGEnemy.h"
#include "EditorEngine.h"
#include "Engine/World.h"
#include "LevelEditor/SLevelEditor.h"


std::shared_ptr<FGameManager> FGameManager::Instance;

FGameManager& FGameManager::Get()
{
    if (!Instance)
    {
        Instance = std::make_shared<FGameManager>();
    }
    return *Instance.get();
}

void FGameManager::BeginPlay()
{
    CurrentGameState= EGameState::Playing;
    Score = 0;
    GameTimer = 0.0f;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            AActor* enemy = GEngine->GetWorld()->SpawnActor<AGEnemy>();
            enemy->SetActorLocation(FVector(i * 10, j * 10, 2));
        }
    }
}

void FGameManager::RestartGame()
{
    CurrentGameState = EGameState::PrepareRestart;
    GEngine->GetLevelEditor()->GetEditorStateManager().SetState(EEditorState::Stopped);
}

void FGameManager::StartGame()
{
    GEngine->GetWorld()->LoadScene("Assets/Scenes/Game.scene");
}

void FGameManager::EndGame()
{
    CurrentGameState  = EGameState::Ended;
    //GEngine->GetWorld()->ReloadScene("Assets/Scenes/EndGame.scene");
}

void FGameManager::SpawnEnemy()
{
    GEngine->GetWorld()->SpawnActor<AGEnemy>()->SetActorLocation(FVector(0, 0, 2));
}

void FGameManager::Tick(float DeltaTime)
{
    UpdateGameTimer( DeltaTime);
    if (GameOverTimer - GameTimer <= 0.0f)
    {
        EndGame();
    }
}

void FGameManager::EditorTick(float DeltaTime)
{
    if (CurrentGameState == EGameState::PrepareRestart)
    {
        GEngine->GetWorld()->ReloadScene("Assets/Scenes/Game.scene");
        GEngine->GetLevelEditor()->GetEditorStateManager().SetState(EEditorState::PreparingPlay);
        GameTimer = 0.0f;
        Score = 0;
        CurrentGameState = EGameState::Playing;
    }
}