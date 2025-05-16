#pragma once
#include <fstream>

#include "Container/String.h"

struct Serializer
{
    /* Write FString */
    static void WriteFString(std::ofstream& Stream, const FString& InString)
    {
        uint32 Length = InString.Len();
        Stream.write(reinterpret_cast<const char*>(&Length), sizeof(Length));
        Stream.write(GetData(InString), Length * sizeof(char));
    }

    /* Read FString */
    static void ReadFString(std::ifstream& Stream, FString& InString)
    {
        uint32 Length = 0;
        Stream.read(reinterpret_cast<char*>(&Length), sizeof(Length));
        char* Buffer = new char[Length + 1];
        Stream.read(Buffer, Length);
        Buffer[Length] = '\0';
        InString = Buffer;
        delete[] Buffer;
    }

    /* Write FWString */
    static void WriteFWString(std::ofstream& Stream, const FWString& InString)
    {
        uint32 Length = static_cast<uint32>(InString.length());
        Stream.write(reinterpret_cast<const char*>(&Length), sizeof(Length));
        Stream.write(reinterpret_cast<const char*>(InString.c_str()), Length * sizeof(wchar_t));
    }

    /* Read FWString */
    static void ReadFWString(std::ifstream& Stream, FWString& InString)
    {
        uint32 Length = 0;
        Stream.read(reinterpret_cast<char*>(&Length), sizeof(Length));
        wchar_t* Buffer = new wchar_t[Length + 1];
        Stream.read(reinterpret_cast<char*>(Buffer), Length * sizeof(wchar_t));
        Buffer[Length] = L'\0';
        InString = Buffer;
        delete[] Buffer;
    }
};
