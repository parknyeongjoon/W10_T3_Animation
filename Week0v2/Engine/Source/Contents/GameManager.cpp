#include "GameManager.h"

#include "AGEnemy.h"
#include "EditorEngine.h"
#include "Engine/World.h"


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
    GEngine->GetWorld()->ReloadScene("Assets/Scenes/Game.scene");
}

void FGameManager::StartGame()
{
    GEngine->GetWorld()->LoadScene("Assets/Scenes/Game.scene");
}

void FGameManager::EndGame()
{
    GEngine->GetWorld()->LoadScene("Assets/Scenes/EndGame.scene");
}

void FGameManager::SpawnEnemy()
{
    GEngine->GetWorld()->SpawnActor<AGEnemy>()->SetActorLocation(FVector(0, 0, 2));
}
