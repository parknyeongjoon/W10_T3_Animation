#include "DummyObject.h"

#include "Engine/Engine.h"
#include "Serialization/Serializer.h"
#include "UObject/Casts.h"

extern UEngine* GEngine;

void DummyTest::PopulateDummyObject2(DummyObject2* dummyObject2)
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


void DummyTest::TestDummyObject2Serialization(UObject* Outer)
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
