#include "GameManager.h"

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
    int Score = 0;
    float GameTimer = 0.0f;
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
