#include "World.h"

#include "Actors/Player.h"
#include "BaseGizmos/TransformGizmo.h"
#include "Camera/CameraComponent.h"
#include "LevelEditor/SLevelEditor.h"
#include "Engine/FLoaderOBJ.h"
#include "Classes/Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Components/SkySphereComponent.h"
#include "UnrealEd/SceneMgr.h"
#include "UObject/UObjectIterator.h"
#include "Level.h"
#include "Serialization/FWindowsBinHelper.h"


UWorld::UWorld(const UWorld& Other): UObject(Other)
                                   , defaultMapName(Other.defaultMapName)
                                   , Level(Other.Level)
                                   , WorldType(Other.WorldType)
                                    , EditorPlayer(Other.EditorPlayer)
{
}

void UWorld::InitWorld()
{
    // TODO: Load Scene
    PreLoadResources();
    CreateBaseObject();
    Level = FObjectFactory::ConstructObject<ULevel>();
}

void UWorld::LoadLevel(const FString& LevelName)
{
    // !TODO : 레벨 로드
    // 이름으로 레벨 로드한다
    // 실패 하면 현재 레벨 유지
}

void UWorld::PreLoadResources()
{
    FManagerOBJ::CreateStaticMesh(TEXT("Assets/CastleObj.obj"));
}

void UWorld::CreateBaseObject()
{
    if (EditorPlayer == nullptr)
    {
        EditorPlayer = FObjectFactory::ConstructObject<AEditorPlayer>();
    }
    
    if (LocalGizmo == nullptr)
    {
        LocalGizmo = FObjectFactory::ConstructObject<UTransformGizmo>();
    }
}

void UWorld::ReleaseBaseObject()
{
    if (LocalGizmo)
    {
        delete LocalGizmo;
        LocalGizmo = nullptr;
    }
    
    if (EditorPlayer)
    {
        delete EditorPlayer;
        EditorPlayer = nullptr;
    }

}

void UWorld::Tick(ELevelTick tickType, float deltaSeconds)
{
    if (tickType == LEVELTICK_ViewportsOnly)
    {
        if (EditorPlayer)
            EditorPlayer->Tick(deltaSeconds);
        if (LocalGizmo)
            LocalGizmo->Tick(deltaSeconds);
    }
    // SpawnActor()에 의해 Actor가 생성된 경우, 여기서 BeginPlay 호출
    if (tickType == LEVELTICK_All)
    {
        for (AActor* Actor : Level->PendingBeginPlayActors)
        {
            Actor->BeginPlay();
        }
        Level->PendingBeginPlayActors.Empty();

        // 매 틱마다 Actor->Tick(...) 호출
        for (AActor* Actor : Level->GetActors())
        {
            Actor->Tick(deltaSeconds);
        }
    }
}

void UWorld::Release()
{
	for (AActor* Actor : Level->GetActors())
	{
		Actor->EndPlay(EEndPlayReason::WorldTransition);
        TArray<UActorComponent*> Components = Actor->GetComponents();
	    for (UActorComponent* Component : Components)
	    {
	        GUObjectArray.MarkRemoveObject(Component);
	    }
	    GUObjectArray.MarkRemoveObject(Actor);
	}
    Level->GetActors().Empty();

	pickingGizmo = nullptr;
	ReleaseBaseObject();

    GUObjectArray.ProcessPendingDestroyObjects();
}

void UWorld::ClearScene()
{
    // 1. PickedActor제거
    SelectedActor = nullptr;
    // 2. 모든 Actor Destroy
    
    for (AActor* actor : TObjectRange<AActor>())
    {
        DestroyActor(actor);
    }
    ReleaseBaseObject();
}

UObject* UWorld::Duplicate() const
{
    UWorld* CloneWorld = FObjectFactory::ConstructObjectFrom<UWorld>(this);
    CloneWorld->DuplicateSubObjects(this);
    CloneWorld->PostDuplicate();
    return CloneWorld;
}

void UWorld::DuplicateSubObjects(const UObject* SourceObj)
{
    UObject::DuplicateSubObjects(SourceObj);
    Level = Cast<ULevel>(Level->Duplicate());
    EditorPlayer = FObjectFactory::ConstructObject<AEditorPlayer>();
}

void UWorld::PostDuplicate()
{
    UObject::PostDuplicate();
}

void UWorld::ReloadScene(const FString& FileName)
{

    ClearScene(); // 기존 오브젝트 제거
    CreateBaseObject();
    FArchive ar;
    FWindowsBinHelper::LoadFromBin(FileName, ar);

    ar >> *this;
}

bool UWorld::DestroyActor(AActor* ThisActor)
{
    if (ThisActor->GetWorld() == nullptr)
    {
        return false;
    }

    if (ThisActor->IsActorBeingDestroyed())
    {
        return true;
    }

    // 액터의 Destroyed 호출
    ThisActor->Destroyed();

    if (ThisActor->GetOwner())
    {
        ThisActor->SetOwner(nullptr);
    }

    TArray<UActorComponent*> Components = ThisActor->GetComponents();
    for (UActorComponent* Component : Components)
    {
        Component->DestroyComponent();
    }

    // World에서 제거
    Level->GetActors().Remove(ThisActor);

    // 제거 대기열에 추가
    GUObjectArray.MarkRemoveObject(ThisActor);
    return true;
}

void UWorld::SetPickingGizmo(UObject* Object)
{
	pickingGizmo = Cast<USceneComponent>(Object);
}

void UWorld::Serialize(FArchive& ar) const
{
    int ActorCount = Level->GetActors().Num();
    ar << ActorCount;
    for (AActor* Actor : Level->GetActors())
    {
        FActorInfo ActorInfo = Actor->GetActorInfo();
        ar << ActorInfo;
    }
}

void UWorld::Deserialize(FArchive& ar)
{
    int ActorCount;
    ar >> ActorCount;
    for (int i = 0; i < ActorCount; i++)
    {
        FActorInfo ActorInfo;
        ar >> ActorInfo;
        UClass* ActorClass = UClassRegistry::Get().FindClassByName(ActorInfo.Type);
        if (ActorClass)
        {
            AActor* Actor = SpawnActorByClass(ActorClass, true);
            if (Actor)
            {
                Actor->LoadAndConstruct(ActorInfo.ComponentInfos);
                Level->GetActors().Add(Actor);
            }
        }
    }
}

/*************************임시******************************/
bool UWorld::IsPIEWorld() const
{
    return false;
}

void UWorld::BeginPlay()
{

}

UWorld* UWorld::DuplicateWorldForPIE(UWorld* world)
{
    return new UWorld();
}
/**********************************************************/
