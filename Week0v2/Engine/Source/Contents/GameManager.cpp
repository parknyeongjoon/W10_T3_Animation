#include "GameManager.h"

#include "AGEnemy.h"
#include "PlayerCameraManager.h"
#include "EditorEngine.h"
#include "Camera/CameraFadeInOut.h"
#include "Camera/CameraLetterBox.h"
#include "Engine/World.h"
#include "LevelEditor/SLevelEditor.h"
#include "Actors/SkeletalMeshActor.h"
#include "Components/PrimitiveComponents/MeshComponents/SkeletalMeshComponent.h"

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
    CurrentGameState = EGameState::Playing;
    CurrentAnimState = ETestState::None;

    GameTimer = 0.0f;
    
    Score = 0;
    Health = 0;
    
    bTrackProcess = false;

    MinSuccessTime = 0.0f;
    MaxSuccessTime = 0.0f;

    bGetTrackScore = true;
    bTrackProcess = false;
    bGameOver = false;

    for (auto& Time : AnimTime)
    {
        Time = 0.0f;
    }

    StartGame();
}

void FGameManager::StartNPCAnimation(int index, ETestState AnimState)
{
    if (NPCs.IsValidIndex(index))
    {
        Cast<UTestAnimInstance>(NPCs[index]->GetSkeletalMeshComponent()->GetAnimInstance())->SetState(AnimState);
    }
}

void FGameManager::StartTrack(float Duration, ETestState AnimState, EEventType EventCase)
{
    if (bTrackProcess == false)
    {
        bGetTrackScore = false;
        bTrackProcess = true;
        float deltaTime;
        switch (EventCase)
        {
        case EEventType::Single:
            deltaTime = Duration / 4;
            for (int i=0;i<4;i++)
            {
                AnimTime[i] = GameTimer + deltaTime * i;
            }
            break;
        case EEventType::Duo:
            deltaTime = Duration / 2;
            AnimTime[0] = GameTimer;
            AnimTime[1] = GameTimer + deltaTime;
            AnimTime[2] = GameTimer + deltaTime;
            AnimTime[3] = GameTimer + deltaTime * 2;
            break;
        case EEventType::Squad:
            deltaTime = Duration;
            AnimTime[0] = GameTimer;
            AnimTime[1] = GameTimer + deltaTime;
            AnimTime[2] = GameTimer + deltaTime;
            AnimTime[3] = GameTimer + deltaTime;
            break;
        }

        MinSuccessTime = AnimTime[3] - deltaTime * 0.05f; 
        MaxSuccessTime = AnimTime[3] + deltaTime * 0.05f; 

        CurrentAnimState = AnimState;
    }

    for (int i=0;i<4;i++)
    {
        if (AnimTime[i] != 0.0f && AnimTime[i] <= GameTimer)
        {
            StartNPCAnimation(i, CurrentAnimState);
            AnimTime[i] = 0.0f;
        }
    }

    if (GameTimer > MaxSuccessTime + 2)
    {
        bTrackProcess = false;
        if (bGetTrackScore == false)
        {
            LoseHealth();
        }
    }
}

void FGameManager::LoseHealth()
{
    Health--;
    if (Health <= 0)
    {
        bGameOver = true;
        GameOverEvent.Broadcast();
        EndGame();
    }
}

void FGameManager::PlayAnimA()
{
    if (bGetTrackScore == false)
    {
        bGetTrackScore = true;
        if (CurrentAnimState == ETestState::Pose && MinSuccessTime < GameTimer && MaxSuccessTime < GameTimer)
        {
            AddScore();
        }
        else
        {
            LoseHealth();
        }
    }
}

void FGameManager::PlayAnimB()
{
    if (bGetTrackScore == false)
    {
        bGetTrackScore = true;
        if (CurrentAnimState == ETestState::Jump && MinSuccessTime < GameTimer && MaxSuccessTime < GameTimer)
        {
            AddScore();
        }
        else
        {
            LoseHealth();
        }
    }
}

void FGameManager::PlayAnimC()
{
    if (bGetTrackScore == false)
    {
        bGetTrackScore = true;
        if (CurrentAnimState == ETestState::Dance && MinSuccessTime < GameTimer && MaxSuccessTime < GameTimer)
        {
            AddScore();
        }
        else
        {
            LoseHealth();
        }
    }
}

void FGameManager::RestartGame()
{
    CurrentGameState = EGameState::PrepareRestart;
    if (UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine))
    {
        EditorEngine->GetLevelEditor()->GetEditorStateManager().SetState(EEditorState::Stopped);
    }
    //
    // UCameraFadeInOut* CameraModifier = FObjectFactory::ConstructObject<UCameraFadeInOut>(PlayerCameraManager);
    // CameraModifier->StartFadeIn(2.0f);
    // PlayerCameraManager->AddCameraModifier(CameraModifier);
}

void FGameManager::StartGame()
{
    CurrentGameState = EGameState::Playing;
}

void FGameManager::EndGame()
{
    CurrentGameState  = EGameState::Ended;
    // UCameraFadeInOut* CameraModifier = FObjectFactory::ConstructObject<UCameraFadeInOut>(PlayerCameraManager);
    // CameraModifier->StartFadeOut(1.0f);
    // PlayerCameraManager->AddCameraModifier(CameraModifier);
    // UCameraLetterBox* CameraLetterBox = FObjectFactory::ConstructObject<UCameraLetterBox>(PlayerCameraManager);
    // CameraLetterBox->DeactivateLetterbox(1.0f);
    // PlayerCameraManager->AddCameraModifier(CameraModifier);
}

void FGameManager::Tick(float DeltaTime)
{
    UpdateGameTimer(DeltaTime);
    if (CurrentGameState == EGameState::Playing && GameTimer > 3.0f && bGameOver == false)
    {
        ETestState AnimState = ETestState::None;
        EEventType EventCase = EEventType::None;
        
        if (CurrentAnimState == ETestState::None) { AnimState = ETestState::Pose; EventCase = EEventType::Single; }
        if (CurrentAnimState == ETestState::Pose) { AnimState = ETestState::Jump; EventCase = EEventType::Duo; }
        if (CurrentAnimState == ETestState::Jump) { AnimState = ETestState::Dance; EventCase = EEventType::Squad; }
        if (CurrentAnimState == ETestState::Dance) { AnimState = ETestState::Pose; EventCase = EEventType::Single; }
        
        StartTrack(3, AnimState, EventCase);
    }
}

void FGameManager::EditorTick(float DeltaTime)
{    
    if (CurrentGameState == EGameState::PrepareRestart)
    {
        GEngine->GetWorld()->LoadScene("Assets/Scenes/Game.scene");
        if (UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine))
        {
            EditorEngine->GetLevelEditor()->GetEditorStateManager().SetState(EEditorState::PreparingPlay);
        }
        GameTimer = 0.0f;
        Score = 0;
        CurrentGameState = EGameState::Playing;
    }
}