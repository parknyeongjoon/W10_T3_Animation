#pragma once
#include "AnimationAsset.h"
#include "AnimTypes.h"
#include "TestFBXLoader.h"
#include "UObject/ObjectMacros.h"

class UAnimDataModel;

class UAnimSequenceBase : public UAnimationAsset
{
    DECLARE_CLASS(UAnimSequenceBase, UAnimationAsset)
public:
    UAnimSequenceBase() = default;
    UAnimSequenceBase(const UAnimSequenceBase& Other);

    virtual UObject* Duplicate(UObject* InOuter) override;
    virtual void DuplicateSubObjects(const UObject* Source, UObject* InOuter) override;
    virtual void PostDuplicate() override;
    PROPERTY(float, RateScale)
    
    UAnimDataModel* GetDataModel() const { return DataModel; }

    void SetData(UAnimDataModel* InDataModel) { DataModel = InDataModel; }
    void SetData(const FString& FilePath);

    /** Sort the Notifies array by time, earliest first. */
    void SortNotifies();
    /** Remove the notifies specified */
    bool RemoveNotifies(const TArray<FName>& NotifiesToRemove);
    /** Remove all notifies */
    void RemoveNotifies();
    /** Renames all named notifies with InOldName to InNewName */
    void RenameNotifies(FName InOldName, FName InNewName);

    virtual void GetAnimationPose(struct FPoseContext& OutPose, const FAnimExtractContext& ExtractionContext) const;
    virtual void EvaluateCurveData(struct FBlendedCurve& OutCurve, const FAnimExtractContext& ExtractionContext) const;

    TArray<FAnimNotifyEvent> Notifies;
protected:
    float RateScale;
    UAnimDataModel* DataModel;
};

