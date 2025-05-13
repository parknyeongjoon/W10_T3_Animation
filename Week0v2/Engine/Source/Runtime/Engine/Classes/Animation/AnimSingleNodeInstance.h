#pragma once
#include "AnimInstance.h"
class UAnimSingleNodeInstance : public UAnimInstance
{
    DECLARE_CLASS(UAnimSingleNodeInstance, UAnimInstance)
public:
    UAnimSingleNodeInstance();
    UAnimSingleNodeInstance(const UAnimSingleNodeInstance& Other);

    virtual UObject* Duplicate(UObject* InOuter) override;
public:
    virtual void SetAnimationAsset(UAnimSequence* NewAsset, bool bIsLooping = true, float InPlayRate = 1.f);

    virtual void NativeInitializeAnimation() override;

    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

    void SetPlaying(bool bIsPlaying)
    {
        bPlaying = bIsPlaying;
    }

    bool IsPlaying() const
    {
        return bPlaying;
    }

    void SetReverse(bool bIsReverse)
    {
        bReverse = bIsReverse;
    }

    bool IsReverse() const
    {
        return bReverse;
    }

    void SetLooping(bool bIsLooping)
    {
        bLooping = bIsLooping;
    }

    bool IsLooping() const
    {
        return bLooping;
    }

    void SetElapsedTime(float InElapsedTime)
    {
        ElapsedTime = InElapsedTime;
    }

    float GetElapsedTime() const
    {
        return ElapsedTime;
    }

    float GetPlayRate() const
    {
        return PlayRate;
    }

    void SetPlayRate(float InPlayRate)
    {
        PlayRate = InPlayRate;
    }

    int32 GetLoopStartFrame() const
    {
        return LoopStartFrame;
    }

    void SetLoopStartFrame(int32 InLoopStartFrame)
    {
        LoopStartFrame = InLoopStartFrame;
    }

    int32 GetLoopEndFrame() const
    {
        return LoopEndFrame;
    }

    void SetLoopEndFrame(int32 InLoopEndFrame)
    {
        LoopEndFrame = InLoopEndFrame;
    }

    int GetCurrentKey() const
    {
        return CurrentKey;
    }

    void SetCurrentKey(int InCurrentKey)
    {
        CurrentKey = InCurrentKey;
    }

    UAnimSequence* GetCurrentAsset() const
    {
        return CurrentAsset;
    }

    void SetCurrentAsset(UAnimSequence* InCurrentAsset)
    {
        CurrentAsset = InCurrentAsset;
    }

private:
    UAnimSequence* CurrentAsset;

    float ElapsedTime;

    float PreviousTime;

    float PlayRate;

    bool bLooping;

    bool bPlaying;

    bool bReverse;

    int32 LoopStartFrame;

    int32 LoopEndFrame;

    int CurrentKey;
};

