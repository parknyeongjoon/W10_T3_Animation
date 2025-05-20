#include "Serializer.h"

#include "Serialization/MemoryArchive.h"
#include "UObject/ObjectFactory.h"

void Serializer::Save(const UObject* Obj, TArray<uint8>& OutBytes)
{
    FMemoryWriter2 Ar(OutBytes);
    Ar.Seek(0);
    
    // 1) 이름 길이 먼저 저장
    FString CName = Obj->GetClass()->GetName();
    int32 NameLen = CName.Len();
    Ar.SerializeRaw(&NameLen, sizeof(NameLen));

    // 2) 문자열(널 포함) 저장
    Ar.SerializeRaw(const_cast<TCHAR*>(*CName), (NameLen + 1) * sizeof(TCHAR));

    const_cast<UObject*>(Obj)->Serialize(Ar);
}

UObject* Serializer::Load(const TArray<uint8>& InBytes)
{
    TArray<uint8> Tmp = InBytes;
    FMemoryReader2 Ar(Tmp);
    Ar.Seek(0);
    // 1) 이름 길이 읽기
    int32 NameLen = 0;
    Ar.SerializeRaw(&NameLen, sizeof(NameLen));

    // 2) 그 길이만큼 버퍼 할당 후 읽기
    TArray<TCHAR> NameBuf;
    NameBuf.SetNum(NameLen + 1);
    Ar.SerializeRaw(NameBuf.GetData(), (NameLen + 1) * sizeof(TCHAR));
    FString ClassName(NameBuf.GetData());
    
    UClass* C = UClass::FindClass(ClassName);
    if (!C)
    {
        return nullptr;
    }
        
    UObject* Obj = FObjectFactory::ConstructObject(C, nullptr);
    Obj->Serialize(Ar);
    return Obj;
}

UObject* Serializer::Duplicate(const UObject* Obj)
{
    TArray<uint8> Buf;
    Save(Obj, Buf);
    return Load(Buf);
}

bool Serializer::SaveToFile(const UObject* Obj, const std::string& FilePath)
{
    TArray<uint8> Buffer;
    Save(Obj, Buffer);

    std::ofstream Out(FilePath, std::ios::binary);
    if (!Out.is_open()) return false;
    Out.write(reinterpret_cast<const char*>(Buffer.GetData()), Buffer.Num());
    return true;
}

UObject* Serializer::LoadFromFile(const std::string& FilePath)
{
    std::ifstream In(FilePath, std::ios::binary | std::ios::ate);
    if (!In.is_open()) return nullptr;

    size_t Size = In.tellg();
    In.seekg(0);

    TArray<uint8> Buffer;
    Buffer.SetNum(Size);
    In.read(reinterpret_cast<char*>(Buffer.GetData()), Size);

    return Load(Buffer);
}
