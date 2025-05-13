#include "AnimInstance.h"
#include "Components/PrimitiveComponents/MeshComponents/SkeletalMeshComponent.h"
#include "UObject/Casts.h"
#include "Animation/AnimSequence.h"
#include "Animation/AnimNotify/AnimNotify.h"
#include "AnimData/AnimDataModel.h"
#include "Math/JungleMath.h"
#include "Components/Mesh/SkeletalMesh.h"
#include "Animation/Skeleton.h"
#include "Animation/AnimationRuntime.h"
UAnimInstance::UAnimInstance(const UAnimInstance& Other) 
    :UObject(Other),
    CurrentSequence(Other.CurrentSequence),
    PreviousSequence(Other.PreviousSequence),
    BlendDuration(Other.BlendDuration)
{
}

UObject* UAnimInstance::Duplicate(UObject* InOuter)
{
    UAnimInstance* NewComp = FObjectFactory::ConstructObjectFrom<UAnimInstance>(this, InOuter);
    NewComp->DuplicateSubObjects(this, InOuter);
    NewComp->PostDuplicate();
    NewComp->SetSkeleton(Cast<USkeletalMeshComponent>(InOuter)->GetSkeletalMesh()->GetSkeleton());
    return NewComp;
}

AActor* UAnimInstance::GetOwningActor() const
{
    return GetOwningComponent()->GetOwner();
}

USkeletalMeshComponent* UAnimInstance::GetOwningComponent() const
{
    return CastChecked<USkeletalMeshComponent>(GetOuter());
}
void UAnimInstance::TriggerAnimNotifies(float DeltaSeconds) const
{
    if (CurrentSequence)
    {
        for (const FAnimNotifyEvent& Notify : CurrentSequence->Notifies)
        {
            // 시간 조건에 맞으면 Notify 실행
            if (Notify.TriggerTime <= CurrentTime && CurrentTime < Notify.TriggerTime + Notify.Duration)
            {
                if (Notify.Notify)
                {
                    Notify.Notify->Notify(GetOwningComponent(), CurrentSequence);
                }
            }
        }
    }
}

void UAnimInstance::NativeUpdateAnimation(float DeltaSeconds) const
{
    TriggerAnimNotifies(DeltaSeconds);
    UpdateCurveValues(DeltaSeconds);
}

void UAnimInstance::UpdateAnimation(UAnimSequence* AnimSequence, float DeltaTime)
{
    const UAnimDataModel* DataModel = AnimSequence->GetDataModel();
    TArray<FName> BoneNames;
    DataModel->GetBoneTrackNames(BoneNames);

    USkeletalMeshComponent* SkeletalMeshComp = GetOwningComponent();
    USkeletalMesh* SkeletalMesh = SkeletalMeshComp->GetSkeletalMesh();

    //for (const auto& Name : BoneNames)
    //{   
    //    FTransform Transform = DataModel->GetBoneTrackTransform(Name, CurrentTime);
    //    FMatrix TransformMatrix = JungleMath::CreateModelMatrix(
    //        Transform.GetLocation(),
    //        Transform.GetRotation(),
    //        Transform.GetScale()
    //    );

    //    int BoneIndex = SkeletalMesh->GetSkeleton()->GetRefSkeletal()->BoneNameToIndexMap[Name.ToString()];
    //    SkeletalMesh->GetRenderData().Bones[BoneIndex].LocalTransform = TransformMatrix;
    //}


    FPoseContext Pose;
    FAnimExtractContext Context(CurrentTime, true, false);
    AnimSequence->GetAnimationPose(Pose, Context);

    for (int32 i = 0; i < SkeletalMesh->GetRenderData().Bones.Num(); ++i)
    {
        const FTransform& BoneTransform = Pose.Pose.BoneTransforms[i];
        FMatrix TransformMatrix = JungleMath::CreateModelMatrix(
            BoneTransform.GetLocation(),
            BoneTransform.GetRotation(),
            BoneTransform.GetScale()
        );
        SkeletalMesh->GetRenderData().Bones[i].LocalTransform = TransformMatrix;
    }


    CurrentTime += DeltaTime;
    if (CurrentTime > DataModel->GetPlayLength())
    {
        CurrentTime = 0.0f;
    }
}

void UAnimInstance::BlendAnimations(UAnimSequence* FromSequence, UAnimSequence* ToSequence, float DeltaTime)
{
    if (!FromSequence || !ToSequence)
        return;

    BlendTime += DeltaTime;
    float BlendAlpha = FMath::Clamp(BlendTime / BlendDuration, 0.0f, 1.0f);
    
    FPoseContext FromPose;
    FPoseContext ToPose;
    FPoseContext OutPose;

    FAnimExtractContext FromContext(CurrentTime, true, false);
    FAnimExtractContext ToContext(0, true, false);


    FromSequence->GetAnimationPose(FromPose, FromContext); 
    ToSequence->GetAnimationPose(ToPose, ToContext); 

    FAnimationRuntime::BlendTwoPosesTogether(FromPose.Pose, ToPose.Pose, 
        FromPose.Curve,ToPose.Curve, BlendAlpha,OutPose.Pose,OutPose.Curve);


    USkeletalMeshComponent* SkeletalMeshComp = GetOwningComponent();
    USkeletalMesh* SkeletalMesh = SkeletalMeshComp->GetSkeletalMesh();

    for (int32 i = 0; i < SkeletalMesh->GetRenderData().Bones.Num(); ++i)
    {
        const FTransform& BoneTransform = OutPose.Pose.BoneTransforms[i];
        FMatrix TransformMatrix = JungleMath::CreateModelMatrix(
            BoneTransform.GetLocation(),
            BoneTransform.GetRotation(),
            BoneTransform.GetScale()
        );
        SkeletalMesh->GetRenderData().Bones[i].LocalTransform = TransformMatrix;
    }

    if (BlendAlpha >= 1.0f)
    {
        bIsBlending = false;
        CurrentTime = 0.0f;
    }
}

void UAnimInstance::UpdateCurveValues(float DeltaSeconds) const
{
    if (CurrentSequence)
    {
        // 현재 애니메이션 시퀀스에서 커브 정보를 가져옴
        //for (const FCurve& Curve : CurrentSequence->Curves)
        //{
        //    // 시간에 따른 커브 값 평가
        //    float Value = Curve.Evaluate(CurrentTime);

        //    // BlendedCurve에 저장
        //    BlendedCurve.SetCurveValue(Curve.CurveName, Value);
        //}
    }
}
