#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

namespace DummyTest
{
    // 전방 선언
struct DummyStruct;

// 테스트용 더미 UObject 서브클래스
class DummyObject : public UObject
{
    DECLARE_CLASS(DummyObject, UObject)
public:
    UPROPERTY(EPropertyFlags::EditAnywhere, int32,   IntValue,     = 0)
    UPROPERTY(EPropertyFlags::EditAnywhere, float,   FloatValue,   = 0.f)
    UPROPERTY(EPropertyFlags::EditAnywhere, double,  DoubleValue,  = 0.0)
    UPROPERTY(EPropertyFlags::EditAnywhere, bool,    BoolValue,    = false)
    UPROPERTY(EPropertyFlags::EditAnywhere, FString, StringValue,  = TEXT(""))

    UPROPERTY(EPropertyFlags::EditAnywhere, FName,   NameValue,    = NAME_None)

    UPROPERTY(EPropertyFlags::EditAnywhere, TArray<int32>,     IntArray,        = {})
    UPROPERTY(EPropertyFlags::EditAnywhere, TSet<FName>,       StringSet,       = {})

    using NameToIntMapType = TMap<FName,int32>;
    UPROPERTY(EPropertyFlags::EditAnywhere, NameToIntMapType, NameToIntMap,  = {})

    DummyObject() = default;
    ~DummyObject() override = default;
};

// 테스트용 더미 UObject 서브클래스
class DummyObject0 : public UObject
{
    DECLARE_CLASS(DummyObject0, UObject)
public:
    UPROPERTY(EPropertyFlags::EditAnywhere, int32,                IntValue0,                = 0)
    UPROPERTY(EPropertyFlags::EditAnywhere, float,                FloatValue0,              = 0.f)
    UPROPERTY(EPropertyFlags::EditAnywhere, double,               DoubleValue0,             = 0.0)
    UPROPERTY(EPropertyFlags::EditAnywhere, bool,                 BoolValue0,               = false)
    UPROPERTY(EPropertyFlags::EditAnywhere, FString,              StringValue0,             = TEXT(""))
    UPROPERTY(EPropertyFlags::EditAnywhere, FName,                NameValue0,               = NAME_None)

    UPROPERTY(EPropertyFlags::EditAnywhere, TArray<int32>,             IntArray0,               = {})
    UPROPERTY(EPropertyFlags::EditAnywhere, TArray<DummyStruct>,       DummyStructArray0,       = {})
    UPROPERTY(EPropertyFlags::EditAnywhere, TSet<FName>,               StringSet0,              = {})

    using NameToIntMapType = TMap<FName,int32>;
    UPROPERTY(EPropertyFlags::EditAnywhere, NameToIntMapType,         StringToIntMap0,         = {})
    using NameToDummyStructType = TMap<FName,DummyStruct>;
    UPROPERTY(EPropertyFlags::EditAnywhere, NameToDummyStructType,   StringToDummyStructMap0, = {})

    DummyObject0() = default;
    ~DummyObject0() override = default;
};

class DummyObject1 : public UObject
{
    DECLARE_CLASS(DummyObject1, UObject)
public:
    UPROPERTY(EPropertyFlags::EditAnywhere, int32,                   IntValue1,           = 0)
    UPROPERTY(EPropertyFlags::EditAnywhere, float,                   FloatValue1,         = 0.f)
    UPROPERTY(EPropertyFlags::EditAnywhere, double,                  DoubleValue1,        = 0.0)
    UPROPERTY(EPropertyFlags::EditAnywhere, bool,                    BoolValue1,          = false)
    UPROPERTY(EPropertyFlags::EditAnywhere, FString,                 StringValue1,        = TEXT(""))
    UPROPERTY(EPropertyFlags::EditAnywhere, FName,                   NameValue1,          = NAME_None)

    UPROPERTY(EPropertyFlags::EditAnywhere, DummyObject0*,           ObjectPtr1,          = nullptr)

    UPROPERTY(EPropertyFlags::EditAnywhere, TArray<int32>,           IntArray1,           = {})
    UPROPERTY(EPropertyFlags::EditAnywhere, TArray<DummyObject0*>,   ObjectArray1,        = {})
    UPROPERTY(EPropertyFlags::EditAnywhere, TSet<FName>,             StringSet1,          = {})

    using NameToIntMapType = TMap<FName,int32>;
    UPROPERTY(EPropertyFlags::EditAnywhere, NameToIntMapType,       StringToIntMap1,     = {})
    using NameToDummyObject0 = TMap<FName,DummyObject0*>;
    UPROPERTY(EPropertyFlags::EditAnywhere, NameToDummyObject0 ,StringToObjectMap1,= {})

    DummyObject1() = default;
    ~DummyObject1() override = default;
};

class DummyObject2 : public UObject
{
    DECLARE_CLASS(DummyObject2, UObject)
public:
    UPROPERTY(EPropertyFlags::EditAnywhere, int32,                   IntValue2,           = 0)
    UPROPERTY(EPropertyFlags::EditAnywhere, float,                   FloatValue2,         = 0.f)
    UPROPERTY(EPropertyFlags::EditAnywhere, double,                  DoubleValue2,        = 0.0)
    UPROPERTY(EPropertyFlags::EditAnywhere, bool,                    BoolValue2,          = false)
    UPROPERTY(EPropertyFlags::EditAnywhere, FString,                 StringValue2,        = TEXT(""))
    UPROPERTY(EPropertyFlags::EditAnywhere, FName,                   NameValue2,          = NAME_None)

    UPROPERTY(EPropertyFlags::EditAnywhere, DummyObject1*,           ObjectPtr2,          = nullptr)

    UPROPERTY(EPropertyFlags::EditAnywhere, TArray<int32>,           IntArray2,           = {})
    UPROPERTY(EPropertyFlags::EditAnywhere, TArray<DummyObject1*>,   ObjectArray2,        = {})
    UPROPERTY(EPropertyFlags::EditAnywhere, TSet<FName>,             StringSet2,          = {})
    
    using NameToIntMapType = TMap<FName,int32>;
    UPROPERTY(EPropertyFlags::EditAnywhere, NameToIntMapType,       StringToIntMap2,     = {})
    using NameToDummyObject1 = TMap<FName,DummyObject1*>;
    UPROPERTY(EPropertyFlags::EditAnywhere, NameToDummyObject1 ,StringToObjectMap2,= {})

    DummyObject2() = default;
    ~DummyObject2() override = default;
};

struct DummyStruct
{
    DECLARE_STRUCT(DummyStruct)

    UPROPERTY(EPropertyFlags::EditAnywhere, int32,                IntValueST,           = 0)
    UPROPERTY(EPropertyFlags::EditAnywhere, float,                FloatValueST,         = 0.f)
    UPROPERTY(EPropertyFlags::EditAnywhere, double,               DoubleValueST,        = 0.0)
    UPROPERTY(EPropertyFlags::EditAnywhere, bool,                 BoolValueST,          = false)
    UPROPERTY(EPropertyFlags::EditAnywhere, FString,              StringValueST,        = TEXT(""))
    UPROPERTY(EPropertyFlags::EditAnywhere, FName,                NameValueST,          = NAME_None)

    UPROPERTY(EPropertyFlags::EditAnywhere, DummyObject*,         ObjectPtrST,          = nullptr)

    UPROPERTY(EPropertyFlags::EditAnywhere, TArray<int32>,        IntArrayST,           = {})
    UPROPERTY(EPropertyFlags::EditAnywhere, TArray<DummyObject*>, ObjectArrayST,        = {})
    UPROPERTY(EPropertyFlags::EditAnywhere, TSet<FName>,          StringSetST,          = {})
    
    using NameToIntMapType = TMap<FName,int32>;
    UPROPERTY(EPropertyFlags::EditAnywhere, NameToIntMapType,    StringToIntMapST,     = {})
    using NameToDummyObject = TMap<FName,DummyObject*>;
    UPROPERTY(EPropertyFlags::EditAnywhere, NameToDummyObject, StringToObjectMapST,= {})

    DummyStruct()
        : IntValueST(0)
        , FloatValueST(0.f)
        , DoubleValueST(0.0)
        , BoolValueST(false)
        , StringValueST(TEXT(""))
        , NameValueST(NAME_None)
        , ObjectPtrST(nullptr)
        , IntArrayST()
        , ObjectArrayST()
        , StringSetST()
        , StringToIntMapST()
        , StringToObjectMapST()
    {}
};
    
    // DummyObject2 의 모든 프로퍼티를 식별 가능한 값으로 채워주는 함수
void PopulateDummyObject2(DummyObject2* dummyObject2);

// 테스트 함수
void TestDummyObject2Serialization(UObject* Outer);

}
