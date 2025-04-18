#pragma once
#include "HAL/PlatformType.h"
//#include "GameFramework/Actor.h"

class FName;
class FString;

struct FParse
{

    /** Parses a string from a text string. 
     * @param Stream, the string you want to extract the value from.
     * @param Match, the identifier for the value in the stream.
     * @param Value, the destination to the value to be extracted to.
     * @param MaxLen, the maximum size eof the string that can be extracted.
     * @param bShouldStopOnSeparator, (default = true) If this is true, and the value doesn't start with a '"'
                then it may be truncated to ',' or ')' in addition to whitespace.
     * @param OptStreamGotTo, (default = nullptr) If this is not null, then its dereference is set to the address
                of the end of the value within Stream. This permits consuming of stream in a loop where Match may
                occur multiple times.
    */
    static bool Value( const TCHAR* Stream, const TCHAR* Match, TCHAR* Value, int32 MaxLen, bool bShouldStopOnSeparator=true, const TCHAR** OptStreamGotTo = nullptr);
	

    /** Parses a name. */
    static  bool Value( const TCHAR* Stream, const TCHAR* Match, FName& Name );
    /** Parses a uint32. */
    static  bool Value( const TCHAR* Stream, const TCHAR* Match, uint32& Value );

    static bool Value( const TCHAR* Stream, const TCHAR* Match, uint8& Value );
    /** Parses a signed byte. */
    static bool Value( const TCHAR* Stream, const TCHAR* Match, int8& Value );
    /** Parses a uint16. */
    static bool Value( const TCHAR* Stream, const TCHAR* Match, uint16& Value );
    /** Parses a signed word. */
    static bool Value( const TCHAR* Stream, const TCHAR* Match, int16& Value );
    /** Parses a floating-point value. */
    static bool Value( const TCHAR* Stream, const TCHAR* Match, float& Value );
    /** Parses a double precision floating-point value. */
    static bool Value(const TCHAR* Stream, const TCHAR* Match, double& Value);
    /** Parses a signed double word. */
    static bool Value( const TCHAR* Stream, const TCHAR* Match, int32& Value );

    /** Parses a string. 
 * @param Stream, the string you want to extract the value from.
 * @param Match, the identifier for the value in the stream.
 * @param Value, the destination to the value to be extracted to.
 * @param bShouldStopOnSeparator, (default = true) If this is true, and the value doesn't start with a '"'
            then it may be truncated to ',' or ')' in addition to whitespace.
 * @param OptStreamGotTo, (default = nullptr) If this is not null, then its dereference is set to the address
            of the end of the value within Stream. This permits consuming of stream in a loop where Match may
            occur multiple times.
*/
    //static bool Value( const TCHAR* Stream, const TCHAR* Match, FString& Value, bool bShouldStopOnSeparator =true, const TCHAR** OptStreamGotTo = nullptr);
    
    /** Parses a boolean value. */
    static bool Bool( const TCHAR* Stream, const TCHAR* Match, bool& OnOff );
    
};
