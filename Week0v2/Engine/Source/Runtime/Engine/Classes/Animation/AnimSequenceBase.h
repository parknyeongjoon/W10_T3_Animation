#pragma once
#include "AnimationAsset.h"
#include "AnimTypes.h"
#include "UObject/ObjectMacros.h"

class UAnimDataModel;
class UAnimSequenceBase : public UAnimationAsset
{
    DECLARE_CLASS(UAnimSequenceBase, UAnimationAsset)
public:
    UAnimSequenceBase() = default;

    PROPERTY(float, RateScale)
    
    UAnimDataModel* GetDataMode() const { return DataModel; }

    /** Sort the Notifies array by time, earliest first. */
    void SortNotifies();
    /** Remove the notifies specified */
    bool RemoveNotifies(const TArray<FName>& NotifiesToRemove);
    /** Remove all notifies */
    void RemoveNotifies();
    /** Renames all named notifies with InOldName to InNewName */
    void RenameNotifies(FName InOldName, FName InNewName);

    TArray<FAnimNotifyEvent> Notifies;
protected:
    float RateScale;
    UAnimDataModel* DataModel;
};

