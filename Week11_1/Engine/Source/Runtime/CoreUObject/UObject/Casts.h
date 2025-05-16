#pragma once
#include <cassert>
#include <concepts>
#include "Object.h"


template<typename To, typename From>
FORCEINLINE To* Cast(From* Src)
{
	// 불완전한 타입 걸러내는 용도
	//static_assert(sizeof(From) > 0 && sizeof(To) > 0, "Attempting to cast between incomplete types");

	if (Src)
	{
		// From이 To를 상속 받았는지? (up casting)
		if constexpr (std::derived_from<From, To>)
		{
			return (To*)Src;
		}
		else
		{
			// Src가 원래 From이었는지? (down casting)
			if (((const UObject*)Src)->IsA<To>())
			{
				return (To*)Src;
			}
		}
	}

	return nullptr;
}

template<typename To, typename From>
FORCEINLINE To* CastChecked(From* Src)
{
	//static_assert(sizeof(From) > 0 && sizeof(To) > 0, "Attempting to cast between incomplete types");
	assert(Src);    // nullptr!

	To* Result = Cast<To>(Src);
	assert(Result); // nullptr!

	return Result;
}
