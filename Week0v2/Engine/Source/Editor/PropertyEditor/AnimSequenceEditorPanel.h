#pragma once
#include "Define.h"
#include "ImGUI/imgui.h"
#include "UnrealEd/EditorPanel.h"

class ULevel;
class UActorComponent;
class USceneComponent;
class USkeletalMeshComponent;
class UAnimDataModel;

class AnimSequenceEditorPanel : public UEditorPanel
{
public:
    void Initialize(float InWidth, float InHeight);
    virtual void Render() override;
    virtual void OnResize(HWND hWnd) override;

    void CreateSequencerButton(ImVec2 ButtonSize) const;

private:
    // @todo Notify Color
    //void RGBToHSV(float r, float g, float b, float& h, float& s, float& v) const;
    //void HSVToRGB(float h, float s, float v, float& r, float& g, float& b) const;

private:
    float Width = 0, Height = 0;

    USkeletalMeshComponent* SkeletalMeshComponent = nullptr;
    //UAnimDataModel* PrevAnimDataModel = nullptr;

    int32 PreviousFrame = 0;
    int32 SelectedTrackIndex_ForRename = INDEX_NONE;
    int32 SelectedNotifyGlobalIndex_ForRename = INDEX_NONE;
    //TCHAR RenameTrackBuffer[256];
    TCHAR RenameNotifyBuffer[256];

    char NewNotifyNameBuffer[128] = "NewNotify";
    float NewNotifyTime = 0.0f;
    float RenameNotifyDuration = 1.0f;
};
