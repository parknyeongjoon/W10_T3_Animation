#include "GameManager.h"

#include "AGEnemy.h"
#include "PlayerCameraManager.h"
#include "EditorEngine.h"
#include "Camera/CameraFadeInOut.h"
#include "Camera/CameraLetterBox.h"
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

    for (auto Actor :GEngine->GetWorld()->GetActors())
    {
        if (APlayerCameraManager* CameraManager = Cast<APlayerCameraManager>(Actor))
        {
            PlayerCameraManager = CameraManager;
        }
    }

    StartGame();
}

void FGameManager::RestartGame()
{
    CurrentGameState = EGameState::PrepareRestart;
    if (UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine))
    {
        EditorEngine->GetLevelEditor()->GetEditorStateManager().SetState(EEditorState::Stopped);
    }

    UCameraFadeInOut* CameraModifier = FObjectFactory::ConstructObject<UCameraFadeInOut>(PlayerCameraManager);
    CameraModifier->StartFadeIn(2.0f);
    PlayerCameraManager->AddCameraModifier(CameraModifier);
}

void FGameManager::StartGame()
{
    //GEngine->GetWorld()->LoadScene("Assets/Scenes/Game.scene");

    UCameraFadeInOut* CameraModifier = FObjectFactory::ConstructObject<UCameraFadeInOut>(PlayerCameraManager);
    CameraModifier->StartFadeIn(2.0f);
    PlayerCameraManager->AddCameraModifier(CameraModifier);
    
}

void FGameManager::EndGame()
{
    CurrentGameState  = EGameState::Ended;

    UCameraFadeInOut* CameraModifier = FObjectFactory::ConstructObject<UCameraFadeInOut>(PlayerCameraManager);
    CameraModifier->StartFadeOut(1.0f);
    PlayerCameraManager->AddCameraModifier(CameraModifier);
    UCameraLetterBox* CameraLetterBox = FObjectFactory::ConstructObject<UCameraLetterBox>(PlayerCameraManager);
    CameraLetterBox->DeactivateLetterbox(1.0f);
    PlayerCameraManager->AddCameraModifier(CameraModifier);
    
    //GEngine->GetWorld()->ReloadScene("Assets/Scenes/EndGame.scene");
}

void FGameManager::SpawnEnemy()
{
    GEngine->GetWorld()->SpawnActor<AGEnemy>()->SetActorLocation(FVector(0, 0, 2));
}

void FGameManager::Tick(float DeltaTime)
{
    UpdateGameTimer( DeltaTime);
    if (GameOverTimer - GameTimer <= 0.0f and CurrentGameState == EGameState::Playing)
    {
        EndGame();
    }
}

void FGameManager::EditorTick(float DeltaTime)
{    
    if (CurrentGameState == EGameState::PrepareRestart)
    {
        GEngine->GetWorld()->ReloadScene("Assets/Scenes/Game.scene");
        if (UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine))
        {
            EditorEngine->GetLevelEditor()->GetEditorStateManager().SetState(EEditorState::PreparingPlay);
        }
        GameTimer = 0.0f;
        Score = 0;
        CurrentGameState = EGameState::Playing;
    }
}