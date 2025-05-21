#pragma once
#include <assert.h>

#include "Archive2.h"
#include "Container/Array.h"
#include "UObject/NameTypes.h"

class FMemoryArchive2 : public FArchive2
{
protected:
    int64 Offset;

    FMemoryArchive2()
        : FArchive2(), Offset(0)
    {}

    virtual int64 Tell() override final
    {
        return Offset;
    }

    virtual FArchive2& operator<<(FName& N) override
    {
        // Serialize the FName as a string
        if (IsLoading())
        {
            FString StringName;
            *this << StringName;
            N = FName(*StringName);
        }
        else
        {
            FString StringName = N.ToString();
            *this << StringName;
        }
        return *this;
    }

    virtual FArchive2& operator<<(UObject*& Object) override
    {
        // FMemoryArchive2로는 UObject 직렬화를 할 수 없음.
        assert(0);
        return *this;
    }
};

class FMemoryWriter2 : public FMemoryArchive2
{
public:
    FMemoryWriter2(TArray<uint8>& InData)
        : FMemoryArchive2()
        , Data(InData)
    {
        bIsSaving = true;
        bIsLoading = false;
    }

    void SaveData(const void* InData, uint64 Length) override
    {
        const int64 StartIndex = Data.Num();

        // 메모리 공간 확보
        Data.AddUninitialized(Length);

        // 데이터 복사
        FPlatformMemory::Memcpy(Data.GetData() + StartIndex, InData, Length);

        // 현재 위치 업데이트
        Offset += Length;
    }

    void Seek(int64 InPos) override
    {
        Offset = InPos;
        if (Offset > Data.Num())
        {
            Data.AddUninitialized(Offset - Data.Num()); // 추가 공간 확보
        }
    }

private:
    TArray<uint8>& Data;
};

class FMemoryReader2 : public FMemoryArchive2
{
public:
    FMemoryReader2(const TArray<uint8>& InData)
        : FMemoryArchive2()
        , Data(InData)
    {
        bIsSaving = false;
        bIsLoading = true;
    }

    void LoadData(void* OutData, uint64 Length) override
    {
        if (Offset + Length > Data.Num())
        {
            throw std::runtime_error("Attempted to read beyond the end of the buffer.");
        }

        // 데이터 복사
        FPlatformMemory::Memcpy(OutData, Data.GetData() + Offset, Length);

        // 현재 위치 업데이트
        Offset += Length;
    }

    void Seek(int64 InPos) override
    {
        if (InPos > Data.Num())
        {
            throw std::runtime_error("Attempted to seek beyond the end of the buffer.");
        }
        Offset = InPos;
    }

private:
    const TArray<uint8>& Data;
};
