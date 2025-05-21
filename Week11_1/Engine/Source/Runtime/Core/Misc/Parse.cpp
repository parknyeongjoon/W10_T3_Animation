#include "Parse.h"
#include <cassert>

#include "Char.h"
#include "Container/CString.h"
#include "Math/MathUtility.h"
#include "Runtime/CoreUObject/UObject/NameTypes.h"


bool FParse::Value(const TCHAR* Stream, const TCHAR* Match, TCHAR* Value, int32 MaxLen, bool bShouldStopOnSeparator, const TCHAR** OptStreamGotTo)
{
    if (MaxLen == 0)
    {
        return false;
    }

    assert(Value && MaxLen > 0);

    const int32 MatchLen = static_cast<int32>(FCString::Strlen(Match));

    if (OptStreamGotTo)
    {
        *OptStreamGotTo = nullptr;
    }

    const TCHAR* FoundInStream = FCString::Strifind(Stream, Match, true);
    if (FoundInStream == nullptr)
    {
        Value[0] = static_cast<TCHAR>('\0');
        return false;
    }

    const TCHAR* ValueStartInStream = FoundInStream + MatchLen;
    const TCHAR* ValueEndInStream;

    // Check for quoted arguments' string with spaces
    // -Option="Value1 Value2"
    //         ^~~~Start
    const bool bArgumentsQuoted = *ValueStartInStream == '"';

    if (bArgumentsQuoted)
    {
        // Skip quote character if only params were quoted.
        ValueStartInStream += 1;
        ValueEndInStream = FCString::Strstr(ValueStartInStream, TEXT("\x22"));

        if (ValueEndInStream == nullptr)
        {
            // this should probably log a warning if bArgumentsQuoted is true, as we started with a '"' and didn't find the terminating one.
            ValueEndInStream = FoundInStream + FCString::Strlen(FoundInStream);
        }
    }
    else
    {
        // Skip initial whitespace
        const TCHAR* WhiteSpaceChars = TEXT(" \r\n\t");
        ValueStartInStream += FCString::Strspn(ValueStartInStream, WhiteSpaceChars);

        // Non-quoted string without spaces.
        const TCHAR* TerminatingChars = bShouldStopOnSeparator ? TEXT(",) \r\n\t") : WhiteSpaceChars;
        ValueEndInStream = ValueStartInStream + FCString::Strcspn(ValueStartInStream, TerminatingChars);
    }

    const int32 ValueLength = FMath::Min<int32>(MaxLen - 1, static_cast<int32>(ValueEndInStream - ValueStartInStream));
    // It is possible for ValueLength to be 0.
    // FCString::Strncpy asserts that its copying at least 1 char, memcpy has no such constraint.
    memcpy(Value, ValueStartInStream, sizeof(Value[0]) * ValueLength);
    Value[ValueLength] = static_cast<TCHAR>('\0');

    if (OptStreamGotTo)
    {
        if (bArgumentsQuoted && *ValueEndInStream == '"')
        {
            ++ValueEndInStream;
        }

        *OptStreamGotTo = ValueEndInStream;
    }

    return true;
}

//
// Get a name.
//

bool FParse::Value(const TCHAR* Stream, const TCHAR* Match, FName& Name)
{
    TCHAR TempStr[NAME_SIZE];

    if (!FParse::Value(Stream, Match, TempStr, NAME_SIZE))
    {
        return false;
    }

    Name = FName(TempStr);

    return true;
}

//
// Get a uint32.
//
bool FParse::Value(const TCHAR* Stream, const TCHAR* Match, uint32& Value)
{
    TCHAR Temp[256];
    if (!FParse::Value(Stream, Match, Temp, std::size(Temp)))
    {
        return false;
    }
    TCHAR* End_NotUsed;

    Value = FCString::Strtoi(Temp, &End_NotUsed, 10);

    return true;
}

//
// Get a byte.
//
bool FParse::Value(const TCHAR* Stream, const TCHAR* Match, uint8& Value)
{
    TCHAR Temp[256];
    if (!FParse::Value(Stream, Match, Temp, std::size(Temp)))
    {
        return false;
    }

    Value = static_cast<uint8>(FCString::Atoi(Temp));
    return Value != 0 || FChar::IsDigit(Temp[0]);
}

//
// Get a signed byte.
//
bool FParse::Value(const TCHAR* Stream, const TCHAR* Match, int8& Value)
{
    TCHAR Temp[256];
    if (!FParse::Value(Stream, Match, Temp, std::size(Temp)))
    {
        return false;
    }

    Value = static_cast<int8>(FCString::Atoi(Temp));
    return Value != 0 || FChar::IsDigit(Temp[0]);
}

//
// Get a word.
//
bool FParse::Value(const TCHAR* Stream, const TCHAR* Match, uint16& Value)
{
    TCHAR Temp[256];
    if (!FParse::Value(Stream, Match, Temp, std::size(Temp)))
    {
        return false;
    }

    Value = static_cast<uint16>(FCString::Atoi(Temp));
    return Value != 0 || FChar::IsDigit(Temp[0]);
}

//
// Get a signed word.
//
bool FParse::Value(const TCHAR* Stream, const TCHAR* Match, int16& Value)
{
    TCHAR Temp[256];
    if (!FParse::Value(Stream, Match, Temp, std::size(Temp)))
    {
        return false;
    }

    Value = static_cast<int16>(FCString::Atoi(Temp));
    return Value != 0 || FChar::IsDigit(Temp[0]);
}

//
// Get a floating-point number.
//
bool FParse::Value(const TCHAR* Stream, const TCHAR* Match, float& Value)
{
    TCHAR Temp[256];
    if (!FParse::Value(Stream, Match, Temp, std::size(Temp)))
    {
        return false;
    }

    Value = FCString::Atof(Temp);
    return true;
}

//
// Get a double precision floating-point number.
//
bool FParse::Value(const TCHAR* Stream, const TCHAR* Match, double& Value)
{
    TCHAR Temp[256];
    if (!FParse::Value(Stream, Match, Temp, std::size(Temp)))
    {
        return false;
    }

    Value = FCString::Atod(Temp);
    return true;
}


//
// Get a signed double word.
//
bool FParse::Value(const TCHAR* Stream, const TCHAR* Match, int32& Value)
{
    TCHAR Temp[256];
    if (!FParse::Value(Stream, Match, Temp, std::size(Temp)))
    {
        return false;
    }

    Value = FCString::Atoi(Temp);
    return true;
}

// bool FParse::Value(const TCHAR* Stream, const TCHAR* Match, FString& Value, bool bShouldStopOnSeparator, const TCHAR** OptStreamGotTo)
// {
//     if (!Stream)
//     {
//         return false;
//     }
//
//     int32 StreamLen = FCString::Strlen(Stream);
//     if (StreamLen > 0)
//     {
//         TArray<TCHAR, TInlineAllocator<4096>> ValueCharArray;
//         ValueCharArray.AddUninitialized(StreamLen + 1);
//         ValueCharArray[0] = TCHAR('\0');
//
//         if( FParse::Value(Stream, Match, ValueCharArray.GetData(), ValueCharArray.Num(), bShouldStopOnSeparator, OptStreamGotTo) )
//         {
//             Value = FString(ValueCharArray.GetData());
//             return true;
//         }
//     }
//
//     return false;
// }


bool FParse::Bool(const TCHAR* Stream, const TCHAR* Match, bool& OnOff)
{
    TCHAR TempStr[16];
    if (FParse::Value(Stream, Match, TempStr, std::size(TempStr)))
    {
        OnOff = FCString::ToBool(TempStr);
        return true;
    }
    else
    {
        return false;
    }
}
