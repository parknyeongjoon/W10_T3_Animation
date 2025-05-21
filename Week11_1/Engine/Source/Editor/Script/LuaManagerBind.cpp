#include "LuaManager.h"

void FLuaManager::BindForwardDeclarations(sol::table& engineTable)
{

    // engineTable.new_usertype<UObject>("UObject",
    //     sol::no_constructor 
    // );
    //
    //
    // ///// UObject 상속
    // engineTable.new_usertype<AActor>("AActor",
    //     sol::no_constructor, 
    //     sol::bases<UObject>()
    // );
    //
    // engineTable.new_usertype<UActorComponent>("UActorComponent",
    //     sol::no_constructor, 
    //     sol::bases<UObject>()
    // );
    //
    // engineTable.new_usertype<UClass>("UClass",
    //     sol::no_constructor, 
    //     sol::bases<UObject>()
    // );
    //
    // engineTable.new_usertype<ULevel>("ULevel",
    //     sol::no_constructor,
    //     sol::bases<UObject>()
    // );
    //
    // engineTable.new_usertype<UMaterial>("UMaterial",
    //     sol::no_constructor, 
    //     sol::bases<UObject>()
    // );
    //
    // engineTable.new_usertype<UStaticMesh>("UStaticMesh",
    //     sol::no_constructor, 
    //     sol::bases<UObject>()
    // );
    //
    // engineTable.new_usertype<UWorld>("UWorld",
    //     sol::no_constructor, 
    //     sol::bases<UObject>()
    // );
    //
    // // --- AActor 상속 ---
    // engineTable.new_usertype<AEditorPlayer>("AEditorPlayer",
    //     sol::no_constructor,
    //     sol::bases<AActor>() 
    // );
    // engineTable.new_usertype<AExponentialHeightFogActor>("AExponentialHeightFogActor",
    //     sol::no_constructor,
    //     sol::bases<AActor>()
    // );
    // engineTable.new_usertype<AFireBall>("AFireBall",
    //     sol::no_constructor,
    //     sol::bases<AActor>()
    // );
    // engineTable.new_usertype<ALight>("ALight",
    //     sol::no_constructor,
    //     sol::bases<AActor>()
    // );
    //
    // /// 라이트 상속
    // {
    //     engineTable.new_usertype<APointLightActor>("APointLightActor",
    // sol::no_constructor,
    // sol::bases<ALight>()
    //     );
    //     engineTable.new_usertype<ADirectionalLightActor>("ADirectionalLightActor",
    // sol::no_constructor,
    // sol::bases<ALight>()
    //     );
    //     engineTable.new_usertype<ASpotLightActor>("ASpotLightActor",
    // sol::no_constructor,
    // sol::bases<ALight>()
    //     );
    // }
    //
    // engineTable.new_usertype<ALuaActor>("ALuaActor",
    //     sol::no_constructor,
    //     sol::bases<AActor>()
    // );
    // engineTable.new_usertype<AStaticMeshActor>("AStaticMeshActor", 
    //     sol::no_constructor,
    //     sol::bases<AActor>()
    // );
    //
    //
    //     // --- UActorComponent 직계 자식 ---
    // engineTable.new_usertype<ULuaComponent>("ULuaComponent", sol::no_constructor, sol::bases<UActorComponent>());
    // engineTable.new_usertype<UMovementComponent>("UMovementComponent", sol::no_constructor, sol::bases<UActorComponent>());
    // engineTable.new_usertype<USceneComponent>("USceneComponent", sol::no_constructor, sol::bases<UActorComponent>());
    //
    // // --- UMovementComponent 자식 ---
    // engineTable.new_usertype<UProjectileMovementComponent>("UProjectileMovementComponent", sol::no_constructor, sol::bases<UMovementComponent>());
    // engineTable.new_usertype<URotatingMovementComponent>("URotatingMovementComponent", sol::no_constructor, sol::bases<UMovementComponent>());
    //
    // // --- USceneComponent 직계 자식 ---
    // engineTable.new_usertype<UCameraComponent>("UCameraComponent", sol::no_constructor, sol::bases<USceneComponent>());
    // engineTable.new_usertype<ULightComponentBase>("ULightComponentBase", sol::no_constructor, sol::bases<USceneComponent>());
    // engineTable.new_usertype<UPrimitiveComponent>("UPrimitiveComponent", sol::no_constructor, sol::bases<USceneComponent>());
    //
    // // --- ULightComponentBase 자식 ---
    // engineTable.new_usertype<ULightComponent>("ULightComponent", sol::no_constructor, sol::bases<ULightComponentBase>());
    //
    // // --- ULightComponent 자식 ---
    // engineTable.new_usertype<UDirectionalLightComponent>("UDirectionalLightComponent", sol::no_constructor, sol::bases<ULightComponent>());
    // engineTable.new_usertype<UPointLightComponent>("UPointLightComponent", sol::no_constructor, sol::bases<ULightComponent>());
    // engineTable.new_usertype<USpotLightComponent>("USpotLightComponent", sol::no_constructor, sol::bases<ULightComponent>());
    //
    // // --- UPrimitiveComponent 직계 자식 ---
    // engineTable.new_usertype<UBillboardComponent>("UBillboardComponent", sol::no_constructor, sol::bases<UPrimitiveComponent>());
    // engineTable.new_usertype<UTextComponent>("UTextComponent", sol::no_constructor, sol::bases<UPrimitiveComponent>()); // 부모 가정
    // engineTable.new_usertype<UExponentialHeightFogComponent>("UExponentialHeightFogComponent", sol::no_constructor, sol::bases<UPrimitiveComponent>()); // 부모 가정
    // engineTable.new_usertype<UHeightFogComponent>("UHeightFogComponent", sol::no_constructor, sol::bases<UPrimitiveComponent>()); // 부모 가정
    // engineTable.new_usertype<UMeshComponent>("UMeshComponent", sol::no_constructor, sol::bases<UPrimitiveComponent>());
    // engineTable.new_usertype<UShapeComponent>("UShapeComponent", sol::no_constructor, sol::bases<UPrimitiveComponent>());
    //
    // // --- UBillboardComponent 자식 ---
    // engineTable.new_usertype<UParticleSubUVComp>("UParticleSubUVComp", sol::no_constructor, sol::bases<UBillboardComponent>()); // 부모 가정
    //
    // // --- UTextComponent 자식 ---
    // engineTable.new_usertype<UTextUUID>("UTextUUID", sol::no_constructor, sol::bases<UTextComponent>()); // 부모 가정
    //
    // // --- UMeshComponent 직계 자식 ---
    // engineTable.new_usertype<UStaticMeshComponent>("UStaticMeshComponent", sol::no_constructor, sol::bases<UMeshComponent>());
    // engineTable.new_usertype<USphereComp>("USphereComp", sol::no_constructor, sol::bases<UMeshComponent>()); // 부모 가정, 이름 USphereComponent 가능성
    //
    // // --- UStaticMeshComponent 자식 ---
    // engineTable.new_usertype<UCubeComp>("UCubeComp", sol::no_constructor, sol::bases<UStaticMeshComponent>()); // 부모 가정
    // engineTable.new_usertype<UGizmoBaseComponent>("UGizmoBaseComponent", sol::no_constructor, sol::bases<UStaticMeshComponent>()); // 부모 가정
    // engineTable.new_usertype<USkySphereComponent>("USkySphereComponent", sol::no_constructor, sol::bases<UStaticMeshComponent>()); // 부모 가정
    //
    // // --- UGizmoBaseComponent 자식 ---
    // engineTable.new_usertype<UGizmoArrowComponent>("UGizmoArrowComponent", sol::no_constructor, sol::bases<UGizmoBaseComponent>());
    // engineTable.new_usertype<UGizmoCircleComponent>("UGizmoCircleComponent", sol::no_constructor, sol::bases<UGizmoBaseComponent>());
    // engineTable.new_usertype<UGizmoRectangleComponent>("UGizmoRectangleComponent", sol::no_constructor, sol::bases<UGizmoBaseComponent>());
    //
    //
    // // --- UShapeComponent 자식 ---
    // engineTable.new_usertype<UBoxShapeComponent>("UBoxShapeComponent", sol::no_constructor, sol::bases<UShapeComponent>()); // 이름 UBoxComponent 가능성
    // engineTable.new_usertype<UCapsuleShapeComponent>("UCapsuleShapeComponent", sol::no_constructor, sol::bases<UShapeComponent>()); // 이름 UCapsuleComponent 가능성
    // engineTable.new_usertype<USphereShapeComponent>("USphereShapeComponent", sol::no_constructor, sol::bases<UShapeComponent>()); // 이름 USphereComponent 가능성
    //
    
}
