#pragma once
#include "AnimationAsset.h"
#include "AnimTypes.h"
#include "Delegates/FFunctor.h"
#include "Engine/FBXLoader.h"
#include "UObject/ObjectMacros.h"

class UAnimDataModel;

class UAnimSequenceBase : public UAnimationAsset
{
    DECLARE_CLASS(UAnimSequenceBase, UAnimationAsset)
public:
    UAnimSequenceBase();

    virtual UObject* Duplicate(UObject* InOuter) override;
    virtual void DuplicateSubObjects(const UObject* Source, UObject* InOuter) override;
    virtual void PostDuplicate() override;
    float RateScale;

public:
    UAnimDataModel* GetDataModel() const { return DataModel; }

    void SetData(UAnimDataModel* InDataModel) { DataModel = InDataModel; }
    void SetData(const FString& FilePath);

    TArray<FAnimNotifyTrack>& GetAnimNotifyTracks() { return AnimNotifyTracks; }
    
    /** Add Notify data from TDelegate */
    void AddNotify(int32 TargetTrackIndex, float Second, TDelegate<void()> OnNotify, float Duration = 0.f, const FName& NotifyName = "Notify");
    /** Add Notify data from function */
    void AddNotify(int32 TargetTrackIndex, float Second, std::function<void()> OnNotify, float Duration = 0.f, const FName& NotifyName = "Notify");
    /** Update Notify data */
    void UpdateNotify(int32 NotifyIndexToUpdate, float NewTriggerTime, float NewDuration, int32 NewTrackIndex, const FName& NewNotifyName);
    /** Sort the Notifies array by time, earliest first. */
    void SortNotifies();
    /** Remove notify specified by index */
    bool RemoveNotifyEvent(int32 NotifyIndexToRemove);
    /** Remove the notifies specified */
    bool RemoveNotifies(const TArray<FName>& NotifiesToRemove);
    /** Remove all notifies */
    void RemoveNotifies();
    /** Renames all named notifies with InOldName to InNewName */
    void RenameNotifies(FName InOldName, FName InNewName);

    void ResetNotifies();

    void AddNotifyTrack(const FName& NotifyTrackName);
    void RemoveNotifyTrack(int32 TrackIndexToRemove);
    void RenameNotifyTrack(int32 TrackIndex, const FName& NewTrackName);

    int32 FindNotifyTrackIndexByName(const FName& NotifyTrackName);

    void GetAnimationPose(struct FPoseContext& OutPose, const FAnimExtractContext& ExtractionContext) const;
    virtual void EvaluateCurveData(struct FBlendedCurve& OutCurve, const FAnimExtractContext& ExtractionContext) const;

    TArray<FAnimNotifyEvent> Notifies;
    TArray<FAnimNotifyTrack> AnimNotifyTracks;
protected:
    UAnimDataModel* DataModel;
};

inline void UAnimSequenceBase::RemoveNotifyTrack(int32 TrackIndexToRemove)
{
    if (!AnimNotifyTracks.IsValidIndex(TrackIndexToRemove))
    {
        return;
    }

    // Remove all notifies associated with this track from the global Notifies array
    // Also, adjust TrackIndex for notifies on tracks that are shifted
    for (int32 NotifyIdx = Notifies.Num() - 1; NotifyIdx >= 0; --NotifyIdx)
    {
        if (Notifies[NotifyIdx].TrackIndex == TrackIndexToRemove)
        {
            Notifies.RemoveAt(NotifyIdx);
        }
        else if (Notifies[NotifyIdx].TrackIndex > TrackIndexToRemove)
        {
            Notifies[NotifyIdx].TrackIndex--;
        }
    }
    AnimNotifyTracks.RemoveAt(TrackIndexToRemove);
}

