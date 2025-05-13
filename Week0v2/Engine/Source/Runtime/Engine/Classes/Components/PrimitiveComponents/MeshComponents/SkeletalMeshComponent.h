#pragma once
#include "MeshComponent.h"
#include "Components/Mesh/SkeletalMesh.h"

class UAnimInstance;
class UStaticMeshComponent;
class UAnimationAsset;
class UAnimSingleNodeInstance;
class UAnimSequence;
class USkeletalMeshComponent : public UMeshComponent
{
    DECLARE_CLASS(USkeletalMeshComponent, UMeshComponent)

public:
    USkeletalMeshComponent() = default;
    USkeletalMeshComponent(const USkeletalMeshComponent& Other);

    virtual UObject* Duplicate(UObject* InOuter) override;
    virtual void DuplicateSubObjects(const UObject* Source, UObject* InOuter) override;
    virtual void PostDuplicate() override;
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime) override;

    void SetData(const FString& FilePath);

    PROPERTY(int, SelectedSubMeshIndex);

    virtual uint32 GetNumMaterials() const override;
    virtual UMaterial* GetMaterial(uint32 ElementIndex) const override;
    virtual uint32 GetMaterialIndex(FName MaterialSlotName) const override;
    virtual TArray<FName> GetMaterialSlotNames() const override;
    virtual void GetUsedMaterials(TArray<UMaterial*>& Out) const override;

    
    virtual int CheckRayIntersection(FVector& rayOrigin, FVector& rayDirection, float& pfNearHitDistance) override;
    
    USkeletalMesh* GetSkeletalMesh() const { return SkeletalMesh; }
    void SetSkeletalMesh(USkeletalMesh* value);
    USkeletalMesh* LoadSkeletalMesh(const FString& FilePath);

    UAnimInstance* GetAnimInstance() const { return AnimInstance; }
    void SetAnimInstance(UAnimInstance* InAnimInstance) { AnimInstance = InAnimInstance; };
    UAnimSingleNodeInstance* GetSingleNodeInstance() const;
    void CreateBoneComponents();
    void UpdateBoneHierarchy();

public:
    void PlayAnimation(UAnimSequence* NewAnimToPlay, bool bLooping);

    void SetAnimation(UAnimSequence* NewAnimToPlay);

    UAnimSequence* GetAnimation() const;

    void Play(bool bLooping);

    void Stop();

    void SetPlaying(bool bPlaying);

    bool IsPlaying() const;

    void SetReverse(bool bIsReverse);

    bool IsReverse() const;

    void SetPlayRate(float Rate);

    float GetPlayRate() const;

    void SetLooping(bool bIsLooping);

    bool IsLooping() const;

    int GetCurrentKey() const;

    void SetCurrentKey(int InKey);

    void SetElapsedTime(float InElapsedTime);

    float GetElapsedTime() const;

    int32 GetLoopStartFrame() const;

    void SetLoopStartFrame(int32 InLoopStartFrame);

    int32 GetLoopEndFrame() const;

    void SetLoopEndFrame(int32 InLoopEndFrame);

    bool bIsAnimationEnabled() const { return bPlayAnimation; }
private:

    bool bPlayAnimation;

private:
    TArray<UStaticMeshComponent*> BoneComponents;
    bool bCPUSkinned = true;

    void SkinningVertex();

protected:
    USkeletalMesh* SkeletalMesh = nullptr;
    UAnimInstance* AnimInstance = nullptr;
    
    int SelectedSubMeshIndex = -1;
    float animTime = 0.f;
};
