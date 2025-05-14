#include "AnimSequenceEditorPanel.h"
#include "ImGUI/imgui.h"
#include "ImGuiNeoSequencer/imgui_neo_sequencer.h"
#include "Font/IconDefs.h"

#include "Engine/FEditorStateManager.h"
#include "Components/PrimitiveComponents/MeshComponents/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimData/AnimDataModel.h"
#include <UObject/UObjectIterator.h>


void AnimSequenceEditorPanel::Initialize(const float InWidth, const float InHeight)
{
    Width = InWidth;
    Height = InHeight;

    // @todo SkeletalMeshComponent 초기화 (월드에서 USkeletalMeshComponent 가져오기, ASkeletalMeshActor 가 1개만 존재함을 가정)
}

void AnimSequenceEditorPanel::Render()
{
    UEditorEngine* EditorEngine = Cast<UEditorEngine>(GEngine);
    if (EditorEngine == nullptr)
    {
        return;
    }

    /* Pre Setup */
    const ImGuiIO& io = ImGui::GetIO();
    ImFont* IconFont = io.Fonts->Fonts[FEATHER_FONT];
    constexpr auto IconSize = ImVec2(32, 32);

    float PanelWidth = (Width) * 0.6f;
    float PanelHeight = (Height) * 0.35f;

    float PanelPosX = (Width - PanelWidth) * 0.5f;
    float PanelPosY = (Height) * 0.575f;

    ImVec2 MinSize(400, 100);
    ImVec2 MaxSize(FLT_MAX, 900);

    /* Min, Max Size */
    ImGui::SetNextWindowSizeConstraints(MinSize, MaxSize);

    /* Panel Position */
    ImGui::SetNextWindowPos(ImVec2(PanelPosX, PanelPosY), ImGuiCond_Always);

    /* Panel Size */
    ImGui::SetNextWindowSize(ImVec2(PanelWidth, PanelHeight), ImGuiCond_Always);

    /* Panel Flags */
    ImGuiWindowFlags PanelFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar;;

    /* Render Start */

    // @todo USkeletalMeshComponent 함수 구현
    // @tpdp Add Notify 및 Notify Track 함수 구현
    for (const auto& it : TObjectRange<USkeletalMeshComponent>())
    {
        SkeletalMeshComponent = it;
    }
    UAnimSequence* AnimSeqence = SkeletalMeshComponent->GetAnimSequence();
    UAnimDataModel* DataModel = AnimSeqence->GetDataModel();
    //SkeletalMeshComponent->TickComponent(0.01f);
    if (!DataModel)
    {
        return;
    }
    if (PrevAnimDataModel != DataModel)
    {
        SkeletalMeshComponent->SetLoopStartFrame(0);
        SkeletalMeshComponent->SetLoopEndFrame(FMath::Max(1, DataModel->GetNumberOfFrames() - 1));
        PrevAnimDataModel = DataModel;
    }

    const int32 FrameRate = DataModel->GetFrameRate().Numerator;    // Number of Frames per second
    const int32 NumFrames = DataModel->GetNumberOfFrames();
    if (NumFrames <= 1)
    {
        ImGui::Begin("Animation Sequence Timeline");
        ImGui::Text("Animation has too few frames.");
        ImGui::End();
        return;
    }

    ImGui::Begin("Sequencer", nullptr, PanelFlags);

    int32 LoopStart = SkeletalMeshComponent->GetLoopStartFrame();
    int32 LoopEnd = SkeletalMeshComponent->GetLoopEndFrame();

    LoopStart = FMath::Clamp(LoopStart, 0, NumFrames - 2);
    LoopEnd = FMath::Clamp(LoopEnd, LoopStart + 1, NumFrames - 1);

    // Check Valid
    if (LoopStart >= LoopEnd || LoopStart < 0 || LoopEnd < 0)
    {
        LoopStart = 0;
        LoopEnd = FMath::Max(1, NumFrames - 1);
        SkeletalMeshComponent->SetLoopStartFrame(LoopStart);
        SkeletalMeshComponent->SetLoopEndFrame(LoopEnd);
    }

    float PlayRate = SkeletalMeshComponent->GetPlayRate();
    bool bLooping = SkeletalMeshComponent->IsLooping();
    bool bReverse = SkeletalMeshComponent->IsReverse();
    bool bPlaying = SkeletalMeshComponent->IsPlaying();
    bool bPlayAnimation = SkeletalMeshComponent->bIsAnimationEnabled();

    float Elapsed = SkeletalMeshComponent->GetElapsedTime();
    float TargetKeyFrame = Elapsed * static_cast<float>(FrameRate);
    int32 CurrentFrame = static_cast<int32>(TargetKeyFrame) % (LoopEnd + 1);
    PreviousFrame = CurrentFrame;

    ImGui::Separator();
    {
        ImGui::Text("Frame: %d / %d", CurrentFrame, NumFrames - 1);
        ImGui::SameLine();
        ImGui::Text("Time: %.2fs", SkeletalMeshComponent->GetElapsedTime());
    }
    ImGui::Separator();
    {
        ImGui::PushFont(IconFont);
        CreateSequencerButton(IconSize);
        ImGui::PopFont();
    }
    ImGui::Separator();
    //const TArray<FAnimNotifyTrack>& Tracks = AnimSeqence->GetAnimNotifyTracks();
    //const TArray<FAnimNotifyEvent>& Events = AnimSeqence->Notifies;

    ImGuiNeoSequencerFlags Flags = ImGuiNeoSequencerFlags_EnableSelection |
                                    ImGuiNeoSequencerFlags_Selection_EnableDragging;

    if (ImGui::BeginNeoSequencer("Sequencer", &CurrentFrame, &LoopStart, &LoopEnd, ImVec2(0, 0), Flags))
    {
        if (CurrentFrame != PreviousFrame)
        {
            SkeletalMeshComponent->SetCurrentKey(CurrentFrame);
            SkeletalMeshComponent->SetElapsedTime(static_cast<float>(CurrentFrame) / static_cast<float>(FrameRate));
        }

        if (ImGui::IsKeyPressed(ImGuiKey_Escape))
        {
            ImGui::ClearNeoKeyframeSelection();
        }

        //int32 PendingRemoveTrackIdx = INDEX_NONE;
        //for (int TrackIdx = 0; TrackIdx < Tracks.Num(); ++TrackIdx)
        //{
        //    bool bOpen = true;
        //    std::string TrackLabel = *Tracks[TrackIdx].TrackName.ToString();
        //    FAnimNotifyTrack& CurrentTrack = AnimSeqence->AnimNotifyTracks[TrackIdx];
        //    ImGui::PushID(TrackIdx);
        //    if (ImGui::BeginNeoGroup(TrackLabel.c_str(), &bOpen))
        //    {
        //        bool bRenamePopup = false;
        //        char trackCtxId[64];
        //        if (ImGui::BeginPopupContextItem(trackCtxId))
        //        {
        //            if (ImGui::MenuItem("Add Track"))
        //            {
        //                int32 NewTrackIdx = INDEX_NONE;
        //                int Index = 0;
        //                FName NewTrackName;
        //                do {
        //                    // Find a unique name from 0 to N
        //                    NewTrackName = FName(*FString::Printf(TEXT("%d"), Index));
        //                    Index++;
        //                } while (AnimSeqence->FindNotifyTrackIndex(NewTrackName) != INDEX_NONE);
        //                AnimSeqence->AddNotifyTrack(NewTrackName, NewTrackIdx);
        //            }
        //            if (ImGui::MenuItem("Remove Track"))
        //            {
        //                PendingRemoveTrackIdx = TrackIdx;
        //                ImGui::CloseCurrentPopup();
        //            }
        //            if (ImGui::MenuItem("Rename Track"))
        //            {
        //                SelectedTrackIndex_ForRename = TrackIdx;
        //                FCString::Strncpy(RenameTrackBuffer, *CurrentTrack.TrackName.ToString(), sizeof(RenameTrackBuffer) / sizeof(TCHAR) - 1);
        //                RenameTrackBuffer[sizeof(RenameTrackBuffer) / sizeof(TCHAR) - 1] = 0;
        //                bRenamePopup = true;
        //            }
        //            if (ImGui::MenuItem("Add Notify"))
        //            {
        //                int32 NewIndex;
        //                AnimSeqence->AddNotifyEvent(TrackIdx, Elapsed, 0, "New Notify", NewIndex);
        //            }
        //            ImGui::EndPopup();
        //        }

        //        // Rename Track Popup
        //        if (bRenamePopup)
        //        {
        //            ImGui::OpenPopup("RenameTrackPopupModal");
        //        }
        //        if (ImGui::BeginPopupModal("RenameTrackPopupModal", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        //        {
        //            ImGui::Text("Rename Track to:");
        //            ImGui::InputText("##NewTrackNameInput", RenameTrackBuffer, sizeof(RenameTrackBuffer) / sizeof(TCHAR));
        //            if (ImGui::Button("OK", ImVec2(120, 0)))
        //            {
        //                if (AnimSeqence->AnimNotifyTracks.IsValidIndex(SelectedTrackIndex_ForRename))
        //                {
        //                    FName NewName(RenameTrackBuffer);
        //                    if (NewName.ToString().Len() > 0)
        //                    {
        //                        AnimSeqence->RenameNotifyTrack(SelectedTrackIndex_ForRename, NewName);
        //                    }
        //                }
        //                ImGui::CloseCurrentPopup();
        //            }
        //            ImGui::SameLine();
        //            if (ImGui::Button("Cancel", ImVec2(120, 0)))
        //            {
        //                ImGui::CloseCurrentPopup();
        //            }
        //            ImGui::EndPopup();
        //        }
        //        if (ImGui::BeginNeoTimelineEx("Notify"))
        //        {
        //            for (int32 Index : Tracks[TrackIdx].NotifyIndices)
        //            {
        //                if (!Events.IsValidIndex(Index))
        //                {
        //                    continue;
        //                }

        //                FAnimNotifyEvent& Notify = AnimSeqence->Notifies[Index];

        //                // 현재 Notify의 프레임 계산
        //                int32 Frame = static_cast<int32>(Notify.Time * static_cast<float>(FrameRate));
        //                int32 OriginalFrame = Frame;
        //                float DurationFrame = Notify.Duration * static_cast<float>(FrameRate);
        //                ImGui::PushID(Index);
        //                if (Notify.IsState())
        //                {
        //                    ImGui::NeoNotifyRange(&Frame, &DurationFrame, IM_COL32(255, 0, 0, 255));
        //                }
        //                else
        //                {
        //                    ImGui::NeoKeyframe(&Frame);
        //                }
        //                ImGui::PopID();
        //                if (ImGui::IsNeoKeyframeRightClicked())
        //                {
        //                    SelectedNotifyGlobalIndex_ForRename = Index;
        //                    FCString::Strncpy(RenameNotifyBuffer, *Notify.NotifyName.ToString(), sizeof(RenameNotifyBuffer) / sizeof(TCHAR) - 1);
        //                    RenameNotifyDuration = Notify.Duration;
        //                    ImGui::OpenPopup("Edit Notify");
        //                }
        //                // 변경 감지 후 업데이트
        //                if (Frame != OriginalFrame)
        //                {
        //                    float NewTime = static_cast<float>(Frame) / FrameRate;
        //                    if (Frame < LoopStart)
        //                    {
        //                        NewTime = LoopStart / FrameRate;
        //                    }
        //                    else if (Frame + DurationFrame > LoopEnd)
        //                    {
        //                        NewTime = (LoopEnd - DurationFrame) / FrameRate;
        //                    }
        //                    AnimSeqence->UpdateNotifyEvent(Index, NewTime, Notify.Duration, Notify.TrackIndex, Notify.NotifyName);
        //                }
        //            }
        //            ImGui::EndNeoTimeLine();
        //        }
        //        if (ImGui::BeginPopupModal("Edit Notify", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        //        {
        //            ImGui::Text("Rename Notify and Duration");
        //            ImGui::InputText("Name", RenameNotifyBuffer, sizeof(RenameNotifyBuffer) / sizeof(TCHAR));
        //            ImGui::InputFloat("Duration", &RenameNotifyDuration, 0.1f);

        //            if (ImGui::Button("OK", ImVec2(120, 0)))
        //            {
        //                if (AnimSeqence->Notifies.IsValidIndex(SelectedNotifyGlobalIndex_ForRename))
        //                {
        //                    FName NewName(RenameNotifyBuffer);
        //                    float NewTime = AnimSeqence->Notifies[SelectedNotifyGlobalIndex_ForRename].Time;
        //                    float MaxEndTime = static_cast<float>(LoopEnd) / static_cast<float>(FrameRate);
        //                    if ((RenameNotifyDuration + NewTime) > MaxEndTime)
        //                    {
        //                        RenameNotifyDuration = MaxEndTime - NewTime;
        //                    }
        //                    int32 TrackIndex = AnimSeqence->Notifies[SelectedNotifyGlobalIndex_ForRename].TrackIndex;

        //                    AnimSeqence->UpdateNotifyEvent(SelectedNotifyGlobalIndex_ForRename, NewTime, RenameNotifyDuration, TrackIndex, NewName);
        //                }
        //                ImGui::CloseCurrentPopup();
        //            }
        //            ImGui::SameLine();
        //            if (ImGui::Button("Delete", ImVec2(120, 0)))
        //            {
        //                if (AnimSeqence->Notifies.IsValidIndex(SelectedNotifyGlobalIndex_ForRename))
        //                {
        //                    AnimSeqence->RemoveNotifyEvent(SelectedNotifyGlobalIndex_ForRename);
        //                }
        //                ImGui::CloseCurrentPopup();
        //            }
        //            ImGui::SameLine();
        //            if (ImGui::Button("Cancel", ImVec2(120, 0)))
        //            {
        //                ImGui::CloseCurrentPopup();
        //            }
        //            ImGui::EndPopup();
        //        }



        //        ImGui::EndNeoGroup();
        //    }
        //    ImGui::PopID();
        //}
        //if (PendingRemoveTrackIdx != INDEX_NONE && AnimSeqence->AnimNotifyTracks.IsValidIndex(PendingRemoveTrackIdx))
        //{
        //    AnimSeqence->RemoveNotifyTrack(PendingRemoveTrackIdx);
        //}
        ImGui::EndNeoSequencer();

    }

    ImGui::End();
}

void AnimSequenceEditorPanel::OnResize(const HWND hWnd)
{
    RECT ClientRect;
    GetClientRect(hWnd, &ClientRect);
    Width = static_cast<float>(ClientRect.right - ClientRect.left);
    Height = static_cast<float>(ClientRect.bottom - ClientRect.top);
}

void AnimSequenceEditorPanel::CreateSequencerButton(ImVec2 ButtonSize) const
{
    /*
    float TotalWidth = ButtonSize.x * 3.0f + 16.0f;
    float ContentWidth = ImGui::GetWindowContentRegionMax().x;

    // 중앙 정렬을 위한 커서 위치 설정
    float CursorPosX = (ContentWidth - TotalWidth) * 0.5f;
    ImGui::SetCursorPosX(CursorPosX);
    */

    bool bLooping = SkeletalMeshComponent->IsLooping();
    bool bReverse = SkeletalMeshComponent->IsReverse();
    bool bPlaying = SkeletalMeshComponent->IsPlaying();

    {
        if (ImGui::Button(ICON_SKIP_BACK, ButtonSize))
        {
            SkeletalMeshComponent->SetCurrentKey(0);
        }
    }
    ImGui::SameLine();
    {
        if (ImGui::Button(ICON_PREV_FRAME, ButtonSize))
        {
            SkeletalMeshComponent->SetCurrentKey(FMath::Max(0, SkeletalMeshComponent->GetCurrentKey() - 1));
        }
    }
    ImGui::SameLine();
    {
        if (bPlaying && bReverse)
        {
            if (ImGui::Button(ICON_PAUSE, ButtonSize))
            {
                SkeletalMeshComponent->SetPlayRate(0.0f);
            }
            ImGui::SameLine();
        }
        else
        {
            if (ImGui::Button(ICON_REVERSE, ButtonSize))
            {
                SkeletalMeshComponent->SetPlayRate(-1.0f);
            }
        }
    }
    ImGui::SameLine();
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        {
            if (ImGui::Button(ICON_RECORD, ButtonSize))
            {
                UE_LOG(LogLevel::Error, "Sequencer Record: Not Implemented!");
            }
        }
        ImGui::PopStyleColor();
    }
    ImGui::SameLine();
    {
        if (bPlaying && !bReverse)
        {
            if (ImGui::Button(ICON_PAUSE, ButtonSize))
            {
                SkeletalMeshComponent->SetPlayRate(0.0f);
            }
            ImGui::SameLine();
        }
        else
        {
            if (ImGui::Button(ICON_PLAY, ButtonSize))
            {
                SkeletalMeshComponent->SetPlayRate(1.0f);
            }
        }
    }
    ImGui::SameLine();
    {
        if (ImGui::Button(ICON_NEXT_FRAME, ButtonSize))
        {
            SkeletalMeshComponent->SetCurrentKey(FMath::Min(SkeletalMeshComponent->GetCurrentKey() + 1, SkeletalMeshComponent->GetLoopEndFrame()));
        }
    }
    ImGui::SameLine();
    {
        if (ImGui::Button(ICON_SKIP_FORWARD, ButtonSize))
        {
            SkeletalMeshComponent->SetCurrentKey(SkeletalMeshComponent->GetLoopEndFrame());
        }
    }
    ImGui::SameLine();
    {
        if (bLooping)
        {
            if (ImGui::Button(ICON_REPEAT, ButtonSize))
            {
                SkeletalMeshComponent->SetLooping(false);
            }
        }
        else
        {
            if (ImGui::Button(ICON_ONCE, ButtonSize))
            {
                SkeletalMeshComponent->SetLooping(true);
            }
        }
    }
}