#include "World.h"

#include "Renderer/Renderer.h"
#include "PlayerCameraManager.h"
#include "BaseGizmos/TransformGizmo.h"
#include "Camera/CameraComponent.h"
#include "LevelEditor/SLevelEditor.h"
#include "Engine/FLoaderOBJ.h"
#include "UObject/UObjectIterator.h"
#include "Level.h"
#include "Engine/FBXLoader.h"
#include "Actors/ADodge.h"
#include "Contents/GameManager.h"
#include "Serialization/FWindowsBinHelper.h"

#include "GameFramework//PlayerController.h"
#include "GameFramework/Character.h"
#include "Particles/ParticleSystemWorldManager.h"
#include "Script/LuaManager.h"
#include "UObject/UObjectArray.h"
#include "UnrealEd/PrimitiveBatch.h"
#include "Components/PrimitiveComponents/ParticleSystemComponent.h"
#include <Particles/Modules/ParticleModuleSpawn.h>
#include "Classes/Particles/ParticleLODLevel.h"
#include "Particles/Modules/ParticleModuleRequired.h"
#include <Particles/Modules/ParticleModuleVelocity.h>
#include <Particles/Modules/ParticleModuleLifetime.h>
#include <Particles/Modules/ParticleModuleLocation.h>
#include <Particles/Modules/ParticleModuleSize.h>

void UWorld::InitWorld()
{
    FParticleSystemWorldManager::OnWorldInit(this);
    
    // TODO: Load Scene
    if (Level == nullptr)
    {
        Level = FObjectFactory::ConstructObject<ULevel>(this);
    }
    PreLoadResources();
    if (WorldType == EWorldType::Editor)
    {
        LoadScene("NewScene.scene");
    }
    else
    {
        CreateBaseObject(WorldType);
    }
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

// DummyObject2 의 모든 프로퍼티를 식별 가능한 값으로 채워주는 함수
void PopulateDummyObject2(DummyObject2* dummyObject2)
{
    dummyObject2 = Cast<DummyObject2>(FObjectFactory::ConstructObject<DummyObject2>(GEngine));
        // 기본 타입s
        dummyObject2->IntValue2    = 200;
        dummyObject2->FloatValue2  = 3.14f;
        dummyObject2->DoubleValue2 = 6.28;
        dummyObject2->BoolValue2   = true;
        dummyObject2->StringValue2 = TEXT("DummyObject2_Value");
        dummyObject2->NameValue2   = FName(TEXT("DummyObject2_Name"));

        // UObject* 포인터
        DummyObject1* child1 = Cast<DummyObject1>(FObjectFactory::ConstructObject<DummyObject1>(dummyObject2));
        child1->IntValue1    = 101;
        child1->StringValue1 = TEXT("Child1");
        // 1) 기본 타입 채우기
        child1->IntValue1    = 101;
        child1->FloatValue1  = 1.01f;
        child1->DoubleValue1 = 1.001;
        child1->BoolValue1   = true;
        child1->StringValue1 = TEXT("Child1_String");
        child1->NameValue1   = FName(TEXT("Child1_Name"));

        // 2) ObjectPtr1 (DummyObject0 포인터) 생성 및 채우기
        DummyObject0* obj0Ptr = Cast<DummyObject0>(
            FObjectFactory::ConstructObject<DummyObject0>(child1)
        );
        // 기본 타입
        obj0Ptr->IntValue0      = 201;
        obj0Ptr->FloatValue0    = 2.01f;
        obj0Ptr->DoubleValue0   = 2.002;
        obj0Ptr->BoolValue0     = true;
        obj0Ptr->StringValue0   = TEXT("Obj0Ptr_String");
        obj0Ptr->NameValue0     = FName(TEXT("Obj0Ptr_Name"));

        // IntArray0
        obj0Ptr->IntArray0 = { 7, 14, 21 };

        // DummyStructArray0
        {
            DummyStruct st;
            st.IntValueST      = 301;
            st.FloatValueST    = 3.01f;
            st.DoubleValueST   = 3.001;
            st.BoolValueST     = true;
            st.StringValueST   = TEXT("StructElem");
            st.NameValueST     = FName(TEXT("Struct_Name"));
            DummyObject* objPtr = Cast<DummyObject>(FObjectFactory::ConstructObject<DummyObject>(obj0Ptr));
            {
                objPtr->IntValue      = 501;
                objPtr->FloatValue    = 5.01f;
                objPtr->DoubleValue   = 5.001;
                objPtr->BoolValue     = true;
                objPtr->StringValue   = TEXT("ObjPtr_String");
                objPtr->NameValue     = FName(TEXT("ObjPtr_Name"));

                // 배열 초기화
                objPtr->IntArray      = { 50, 100, 150 };

                // 셋 초기화
                objPtr->StringSet.Add(FName(TEXT("SetX")));
                objPtr->StringSet.Add(FName(TEXT("SetY")));

                // 맵 초기화
                objPtr->NameToIntMap.Add(FName(TEXT("MapKeyA")), 600);
                objPtr->NameToIntMap.Add(FName(TEXT("MapKeyB")), 700);
            }

            st.ObjectPtrST     =  objPtr;
            st.IntArrayST      = { 1, 2, 3 };
            st.ObjectArrayST   = {};
            st.StringSetST.Add(FName(TEXT("SS_A")));
            st.StringSetST.Add(FName(TEXT("SS_B")));
            st.StringToIntMapST.Add(FName(TEXT("MKey")), 500);
            // StringToObjectMapST은 nullptr로 두거나 할당
            obj0Ptr->DummyStructArray0.Add(st);
        }

        // StringSet0
        obj0Ptr->StringSet0.Add(FName(TEXT("Set0_A")));
        obj0Ptr->StringSet0.Add(FName(TEXT("Set0_B")));

        // StringToIntMap0
        obj0Ptr->StringToIntMap0.Add(FName(TEXT("Key0_A")),  1001);
        obj0Ptr->StringToIntMap0.Add(FName(TEXT("Key0_B")),  1002);

        // StringToDummyStructMap0
        {
            DummyStruct mapSt;
            mapSt.IntValueST          = 401;
            mapSt.FloatValueST        = 4.01f;
            mapSt.DoubleValueST       = 4.001;
            mapSt.BoolValueST         = true;
            mapSt.StringValueST       = TEXT("MapStruct");
            mapSt.NameValueST         = FName(TEXT("MapStruct_Name"));

            // ObjectPtrST 할당 및 초기화
            DummyObject* structObjPtr = Cast<DummyObject>(
                FObjectFactory::ConstructObject<DummyObject>(obj0Ptr)
            );
            structObjPtr->IntValue    = 901;
            structObjPtr->FloatValue  = 9.01f;
            structObjPtr->DoubleValue = 9.001;
            structObjPtr->BoolValue   = true;
            structObjPtr->StringValue = TEXT("MapStructObj");
            structObjPtr->NameValue   = FName(TEXT("MapStructObj_Name"));
            mapSt.ObjectPtrST         = structObjPtr;

            // 배열 초기화
            mapSt.IntArrayST          = { 4, 8, 12 };

            // UObject* 배열 (빈 상태로 두거나 예시 추가 가능)
            mapSt.ObjectArrayST       = {};

            // Set 초기화
            mapSt.StringSetST.Add(FName(TEXT("MS_A")));
            mapSt.StringSetST.Add(FName(TEXT("MS_B")));

            // Map<String,Int> 초기화
            mapSt.StringToIntMapST.Add(FName(TEXT("KeyA")),  700);
            mapSt.StringToIntMapST.Add(FName(TEXT("KeyB")),  800);

            // Map<String,Object> 초기화
            DummyObject* mapObjX = Cast<DummyObject>(FObjectFactory::ConstructObject<DummyObject>(obj0Ptr));
            mapObjX->IntValue      = 1001;
            mapObjX->FloatValue    = 10.01f;
            mapObjX->DoubleValue   = 10.001;
            mapObjX->BoolValue     = false;
            mapObjX->StringValue   = TEXT("MapObjX");
            mapObjX->NameValue     = FName(TEXT("MapObjX_Name"));

            // 배열 초기화
            mapObjX->IntArray      = { 100, 200, 300 };

            // 셋 초기화
            mapObjX->StringSet.Add(FName(TEXT("SetX1")));
            mapObjX->StringSet.Add(FName(TEXT("SetX2")));

            // 맵 초기화
            mapObjX->NameToIntMap.Add(FName(TEXT("KeyX1")), 9001);
            mapObjX->NameToIntMap.Add(FName(TEXT("KeyX2")), 9002);
            mapSt.StringToObjectMapST.Add(FName(TEXT("ObjKeyX")), mapObjX);

            // 최종적으로 DummyObject0의 맵에 추가
            obj0Ptr->StringToDummyStructMap0.Add(FName(TEXT("MapKey")), mapSt);
        }
        child1->ObjectPtr1    = obj0Ptr;

        // 3) IntArray1
        child1->IntArray1 = { 11, 22, 33 };

        // 4) ObjectArray1 (DummyObject0* 배열)
        for (int i = 0; i < 2; ++i)
        {
            DummyObject0* elem = Cast<DummyObject0>(
                FObjectFactory::ConstructObject<DummyObject0>(child1)
            );
            elem->IntValue0    = 301 + i;
            elem->StringValue0 = FString::Printf(TEXT("ArrayElem_%d"), i);
            child1->ObjectArray1.Add(elem);
        }

        // 5) StringSet1
        child1->StringSet1.Add(FName(TEXT("Set_A")));
        child1->StringSet1.Add(FName(TEXT("Set_B")));
        child1->StringSet1.Add(FName(TEXT("Set_C")));

        // 6) StringToIntMap1
        child1->StringToIntMap1.Add(FName(TEXT("MapKey1")), 1001);
        child1->StringToIntMap1.Add(FName(TEXT("MapKey2")), 1002);

        // 7) StringToObjectMap1
        for (int i = 0; i < 2; ++i)
        {
            DummyObject0* mapObj = Cast<DummyObject0>(
                FObjectFactory::ConstructObject<DummyObject0>(child1)
            );
            mapObj->IntValue0    = 401 + i;
            mapObj->StringValue0 = FString::Printf(TEXT("MapObj_%d"), i);
            child1->StringToObjectMap1.Add(
                FName(*FString::Printf(TEXT("ObjKey_%d"), i)),
                mapObj
            );
        }
        
        dummyObject2->ObjectPtr2 = child1;

        // IntArray2
        dummyObject2->IntArray2 = {10, 20, 30};

        // ObjectArray2
        DummyObject1* childA = Cast<DummyObject1>(
            FObjectFactory::ConstructObject<DummyObject1>(dummyObject2)
        );
        childA->IntValue1 = 111;
        dummyObject2->ObjectArray2.Add(childA);

        DummyObject1* childB = Cast<DummyObject1>(
            FObjectFactory::ConstructObject<DummyObject1>(dummyObject2)
        );
        childB->IntValue1 = 112;
        dummyObject2->ObjectArray2.Add(childB);

        // StringSet2
        dummyObject2->StringSet2.Add(FName(TEXT("SetValueA")));
        dummyObject2->StringSet2.Add(FName(TEXT("SetValueB")));
        dummyObject2->StringSet2.Add(FName(TEXT("SetValueC")));

        // StringToIntMap2
        dummyObject2->StringToIntMap2.Add(FName(TEXT("KeyOne")),   1);
        dummyObject2->StringToIntMap2.Add(FName(TEXT("KeyTwo")),   2);
        dummyObject2->StringToIntMap2.Add(FName(TEXT("KeyThree")), 3);

        // StringToObjectMap2
        DummyObject1* mapObj1 = Cast<DummyObject1>(
            FObjectFactory::ConstructObject<DummyObject1>(dummyObject2)
        );
        mapObj1->IntValue1    = 201;
        mapObj1->StringValue1 = TEXT("MapObj1");
        dummyObject2->StringToObjectMap2.Add(FName(TEXT("MapKey1")), mapObj1);

        DummyObject1* mapObj2 = Cast<DummyObject1>(
            FObjectFactory::ConstructObject<DummyObject1>(dummyObject2)
        );
        mapObj2->IntValue1    = 202;
        mapObj2->StringValue1 = TEXT("MapObj2");
        dummyObject2->StringToObjectMap2.Add(FName(TEXT("MapKey2")), mapObj2);
}

// 테스트 함수
void TestDummyObject2Serialization(UObject* Outer)
{
    // 1) 로드 시도
    DummyObject2* dummyObject2 = Cast<DummyObject2>(Serializer::LoadFromFile(TEXT("Contents/Particles/DummyObject.uasset")));

    // 2) 없으면 생성 및 채우기
    if (!dummyObject2)
    {
        dummyObject2 = Cast<DummyObject2>(
            FObjectFactory::ConstructObject<DummyObject2>(Outer)
        );
        PopulateDummyObject2(dummyObject2);
    }

    // 3) 파일로 저장
    const bool bOk = Serializer::SaveToFile(dummyObject2, TEXT("Contents/Particles/DummyObject.uasset"));
    UE_LOG(LogLevel::Display, TEXT("Save %s DummyObject2"), bOk ? TEXT("Succeeded") : TEXT("Failed"));
}

void UWorld::CreateBaseObject(EWorldType::Type WorldType)
{
    if (WorldType == EWorldType::PIE)
    {
        return;
    }

    if (LocalGizmo == nullptr && WorldType)
    {
        LocalGizmo = FObjectFactory::ConstructObject<UTransformGizmo>(this);

        // TODO : 삭제해야 될 Test 코드
        //TestDummyObject2Serialization(GEngine);

        AActor* TestActor = SpawnActor<AActor>();
        UParticleSystemComponent* TestComp = TestActor->AddComponent<UParticleSystemComponent>(EComponentOrigin::Runtime);
        TestComp->Template = FObjectFactory::ConstructObject<UParticleSystem>(this);
        UParticleEmitter* NewEmitter = FObjectFactory::ConstructObject<UParticleEmitter>(nullptr);
        UParticleLODLevel* NewLODLevel = FObjectFactory::ConstructObject<UParticleLODLevel>(nullptr);
        
        NewLODLevel->RequiredModule = FObjectFactory::ConstructObject<UParticleModuleRequired>(nullptr);
        NewLODLevel->Modules.Add(NewLODLevel->RequiredModule);
        NewLODLevel->Modules.Add(FObjectFactory::ConstructObject<UParticleModuleSpawn>(nullptr));
        NewLODLevel->Modules.Add(FObjectFactory::ConstructObject<UParticleModuleVelocity>(nullptr));
        NewLODLevel->Modules.Add(FObjectFactory::ConstructObject<UParticleModuleLifeTime>(nullptr));
        NewLODLevel->Modules.Add(FObjectFactory::ConstructObject<UParticleModuleLocation>(nullptr));
        NewLODLevel->Modules.Add(FObjectFactory::ConstructObject<UParticleModuleSize>(nullptr));

        NewEmitter->LODLevels.Add(NewLODLevel);
        TestComp->Template->Emitters.Add(NewEmitter);
        TestComp->Activate();

        for (auto& Module : NewLODLevel->Modules)
        {
            Module->InitializeDefaults();
        }
        
        
        
    }
}


void UWorld::ReleaseBaseObject()
{
    LocalGizmo = nullptr;
}

void UWorld::Tick(ELevelTick tickType, float deltaSeconds)
{
    if (tickType == LEVELTICK_ViewportsOnly)
    {
        if (LocalGizmo)
        {
            LocalGizmo->Tick(deltaSeconds);
        }
        
        FGameManager::Get().EditorTick(deltaSeconds);
    }
    // SpawnActor()에 의해 Actor가 생성된 경우, 여기서 BeginPlay 호출
    if (tickType == LEVELTICK_All)
    {
        FLuaManager::Get().BeginPlay();
        TSet<AActor*> PendingActors = Level->PendingBeginPlayActors;
        for (AActor* Actor : PendingActors)
        {
            Actor->BeginPlay();
            Level->PendingBeginPlayActors.Remove(Actor);
        }

        TArray CopyActors = Level->GetActors();
        for (AActor* Actor : CopyActors)
        {
            Actor->Tick(deltaSeconds);
        }

        FGameManager::Get().Tick(deltaSeconds);
    }
    
    FParticleSystemWorldManager::Get(this)->Tick(deltaSeconds, tickType);
}

void UWorld::Release()
{
    FParticleSystemWorldManager::OnWorldCleanup(this);
    
    if (WorldType == EWorldType::Editor)
    {
        SaveScene("Assets/Scenes/AutoSave.Scene");
    }
    TArray<AActor*> Actors = Level->GetActors();
    for (AActor* Actor : Actors)
    {
        Actor->Destroy();
    }
    if (LocalGizmo)
    {
        LocalGizmo->Destroy();
    }

    GUObjectArray.MarkRemoveObject(Level);
    // TODO Level -> Release로 바꾸기
    // Level->Release();
    GUObjectArray.MarkRemoveObject(this);

	pickingGizmo = nullptr;
	ReleaseBaseObject();
}

void UWorld::ClearScene()
{
    // 1. PickedActor제거
    SelectedActors.Empty();
    // 2. 모든 Actor Destroy
    
    for (AActor* actor : TObjectRange<AActor>())
    {
        if (actor->GetWorld() == this)
        {
            DestroyActor(actor);
        }
    }
    Level->GetActors().Empty();
    Level->PendingBeginPlayActors.Empty();
    ReleaseBaseObject();
}

UObject* UWorld::Duplicate(UObject* InOuter)
{
    UWorld* CloneWorld = Cast<ThisClass>(Super::Duplicate(InOuter));
    CloneWorld->DuplicateSubObjects(this, InOuter);
    CloneWorld->PostDuplicate();
    return CloneWorld;
}

void UWorld::DuplicateSubObjects(const UObject* SourceObj, UObject* InOuter)
{
    UObject::DuplicateSubObjects(SourceObj, InOuter);
    Level = Cast<ULevel>(Level->Duplicate(this));
    LocalGizmo = FObjectFactory::ConstructObject<UTransformGizmo>(this);
}

void UWorld::PostDuplicate()
{
    UObject::PostDuplicate();
}

UWorld* UWorld::GetWorld() const
{
    return const_cast<UWorld*>(this);
}

void UWorld::LoadScene(const FString& FileName)
{
    ClearScene(); // 기존 오브젝트 제거
    CreateBaseObject(WorldType);
    FArchive ar;
    FWindowsBinHelper::LoadFromBin(FileName, ar);

    ar >> *this;
}

AActor* UWorld::SpawnActorByClass(UClass* ActorClass, UObject* InOuter, bool bCallBeginPlay)
{
    if (ActorClass == nullptr)
        return nullptr;

    AActor* Actor = Cast<AActor>(ActorClass->GetDefaultObject());
    if (Actor == nullptr)
        return nullptr;


    if (bCallBeginPlay)
    {
        Level->PendingBeginPlayActors.Add(Actor);
    }
    else
    {
        Level->GetActors().Add(Actor);
    }

    return Actor;
}

void UWorld::DuplicateSelectedActors()
{
    TSet<AActor*> newSelectedActors;
    for (AActor* Actor : SelectedActors)
    {
        AActor* DupedActor = Cast<AActor>(Actor->Duplicate(this));
        FString TypeName = DupedActor->GetActorLabel().Left(DupedActor->GetActorLabel().Find("_", ESearchCase::IgnoreCase,ESearchDir::FromEnd));
        DupedActor->SetActorLabel(TypeName);
        FVector DupedLocation = DupedActor->GetActorLocation();
        DupedActor->SetActorLocation(FVector(DupedLocation.X+50, DupedLocation.Y+50, DupedLocation.Z));
        Level->GetActors().Add(DupedActor);
        Level->PendingBeginPlayActors.Add(DupedActor);
        newSelectedActors.Add(DupedActor);
    }
    SelectedActors = newSelectedActors;
}
void UWorld::DuplicateSelectedActorsOnLocation()
{
    TSet<AActor*> newSelectedActors;
    for (AActor* Actor : SelectedActors)
    {
        AActor* DupedActor = Cast<AActor>(Actor->Duplicate(this));
        FString TypeName = DupedActor->GetActorLabel().Left(DupedActor->GetActorLabel().Find("_", ESearchCase::IgnoreCase,ESearchDir::FromEnd));
        DupedActor->SetActorLabel(TypeName);
        Level->GetActors().Add(DupedActor);
        Level->PendingBeginPlayActors.Add(DupedActor);
        newSelectedActors.Add(DupedActor);
    }
    SelectedActors = newSelectedActors;
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
        FActorInfo ActorInfo = (Actor->GetActorInfo());
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
            AActor* Actor = SpawnActorByClass(ActorClass, this, true);
            if (Actor)
            {
                Actor->LoadAndConstruct(ActorInfo.ComponentInfos);
                Level->GetActors().Add(Actor);
            }
        }
    }
    Level->PostLoad();
    
}

/*************************임시******************************/
bool UWorld::IsPIEWorld() const
{
    return false;
}

void UWorld::BeginPlay()
{
    // FGameManager::Get().BeginPlay();

    if (PlayerController == nullptr)
    {
        PlayerController = SpawnActor<APlayerController>();

        bool bCharacterExist = false;
        for (AActor* Actor : Level->GetActors())
        {
            if (ACharacter* Character = Cast<ACharacter>(Actor))
            {
                bCharacterExist = true;
                PlayerController->Possess(Character);
                break;
            }
        }
        
        if (bCharacterExist == false)
        {
            ACharacter* Character = SpawnActor<ACharacter>();
            PlayerController->Possess(Character);
            Character->SetActorScale(FVector(0.2f, 0.2f, 0.2f));
        }
        
        APlayerCameraManager* PlayerCameraManager = SpawnActor<APlayerCameraManager>();
        PlayerController->SetPlayerCameraManager(PlayerCameraManager);
    }
}

// AActor* SpawnActorByName(const FString& ActorName, UObject* InOuter, bool bCallBeginPlay)
// {
//     // GetWorld 문제 발생 여지 많음.
//     UClass* ActorClass = UClassRegistry::Get().FindClassByName(ActorName);
//     return GetWorld()->SpawnActorByClass(ActorClass, InOuter, bCallBeginPlay);
// }

/**********************************************************/
