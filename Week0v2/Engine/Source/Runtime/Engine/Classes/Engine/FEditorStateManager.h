#pragma once
#include "EngineBaseTypes.h"
#include "EditorEngine.h"
#include "UObject/Casts.h"
#include "UserInterface/Console.h"

extern UEngine* GEngine;

class FEditorStateManager
{
public:
    FEditorStateManager() = default;
    ~FEditorStateManager() = default;
    // static FEditorStateManager& Get()
    // {
    //     static FEditorStateManager Instance;
    //     return Instance;
    // }

    EEditorState GetEditorState() const { return CurrentState; }
    void SetState(EEditorState NewState);
    bool IsPIERunning() 
    { 
        return 
        CurrentState == EEditorState::Playing || 
        CurrentState == EEditorState::Paused || 
        CurrentState == EEditorState::Resuming; 
    }

    bool IsPaused() { return CurrentState == EEditorState::Paused; }

private:

    bool IsValidTransition(EEditorState To);

    std::string ToString(EEditorState State)
    {
        switch (State)
        {
        case EEditorState::Editing:        return "Editing";
        case EEditorState::PreparingPlay:  return "PreparingPlay";
        case EEditorState::Playing:        return "Playing";
        case EEditorState::Paused:         return "Paused";
        case EEditorState::Resuming:       return "Resuming";
        case EEditorState::Stopped:        return "Stopped";
        default:                           return "Unknown";
        }
    }

private:
    EEditorState CurrentState = EEditorState::Editing;
public:
};

inline void FEditorStateManager::SetState(EEditorState NewState)
{
    if (!IsValidTransition(NewState))
    {
        Console::GetInstance().AddLog(LogLevel::Error, "Try to change inValid state : from %s to %s", ToString(CurrentState).c_str(), ToString(NewState).c_str());
        return;
    }

    CurrentState = NewState;
    Console::GetInstance().AddLog(LogLevel::Display, "Change state : %s", ToString(CurrentState).c_str());

    UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
    
    switch (NewState)
    {
    case EEditorState::Editing:
        break;

    case EEditorState::PreparingPlay:
        // Connect to play button
            if (EditorEngine->LevelType == LEVELTICK_PauseTick)
            {
                EditorEngine->LevelType = LEVELTICK_All;
                return;
            }
        EditorEngine->PreparePIE();      // 추후 Prepare에 실패했을 때 고려 할 수 있어야 할듯
        SetState(EEditorState::Playing);
        return;

    case EEditorState::Playing:     // auto Transition
            
        EditorEngine->StartPIE();
        break;

    case EEditorState::Paused:      // Connect to pause button
        EditorEngine->PausedPIE();
        break;

    case EEditorState::Resuming:    // Connect to resume button
        EditorEngine->ResumingPIE();
        break;

    case EEditorState::Stopped:     // Connect to stop button
        EditorEngine->StopPIE();
        SetState(EEditorState::Editing);
        return;
    }
}

inline bool FEditorStateManager::IsValidTransition(EEditorState To)
{
    switch (CurrentState)
    {
    case EEditorState::Editing:
        return To == EEditorState::PreparingPlay;

    case EEditorState::PreparingPlay:
        return To == EEditorState::Playing;

    case EEditorState::Playing:
        return To == EEditorState::Paused || To == EEditorState::Stopped;

    case EEditorState::Paused:
        return To == EEditorState::Resuming || To == EEditorState::Stopped || To == EEditorState::Playing|| To == EEditorState::Paused;

    case EEditorState::Resuming:
        return To == EEditorState::Playing || To == EEditorState::Stopped;

    case EEditorState::Stopped:
        return To == EEditorState::Editing;
    }
}

