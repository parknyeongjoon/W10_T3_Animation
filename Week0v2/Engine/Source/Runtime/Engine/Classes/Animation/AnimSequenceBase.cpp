#include "AnimSequenceBase.h"

#include "AnimData/AnimDataModel.h"

void UAnimSequenceBase::SetData(const FString& FilePath)
{
    SetData(TestFBXLoader::GetAnimData(FilePath));
}

void UAnimSequenceBase::SortNotifies()
{
    Notifies.Sort();
}

bool UAnimSequenceBase::RemoveNotifies(const TArray<FName>& NotifiesToRemove)
{
    bool bSequenceModified = false;
    for (int32 NotifyIndex = Notifies.Num() - 1; NotifyIndex >= 0; --NotifyIndex)
    {
        FAnimNotifyEvent& AnimNotify = Notifies[NotifyIndex];
        if (NotifiesToRemove.Contains(AnimNotify.NotifyName))
        {
            Notifies.RemoveAt(NotifyIndex);
            bSequenceModified = true;
        }
    }
    
    return bSequenceModified;
}

void UAnimSequenceBase::RemoveNotifies()
{
    if (Notifies.Num() == 0)
    {
        return;
    }

    Notifies.Empty();
}

void UAnimSequenceBase::RenameNotifies(FName InOldName, FName InNewName)
{
    for(FAnimNotifyEvent& Notify : Notifies)
    {
        if(Notify.NotifyName == InOldName)
        {
            Notify.NotifyName = InNewName;
        }
    }
}
