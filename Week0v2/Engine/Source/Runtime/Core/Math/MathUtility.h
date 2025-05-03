#pragma once
#include <cmath>
#include <numbers>
#include "Core/HAL/PlatformType.h"


#define PI                   (3.1415926535897932f)
#define SMALL_NUMBER         (1.e-8f)
#define KINDA_SMALL_NUMBER   (1.e-4f)

#define PI_DOUBLE            (3.141592653589793238462643383279502884197169399)


struct FMath
{
	/** A와 B중에 더 작은 값을 반환합니다. */
	template <typename T>
	[[nodiscard]] static FORCEINLINE constexpr T Min(const T A, const T B)
	{
		return A < B ? A : B;
	}

	/** A와 B중에 더 큰 값을 반환합니다. */
	template <typename T>
	[[nodiscard]] static FORCEINLINE constexpr T Max(const T A, const T B)
	{
		return B < A ? A : B;
	}

	/** X를 Min과 Max의 사이의 값으로 클램핑 합니다. */
	template <typename T>
	[[nodiscard]] static FORCEINLINE constexpr T Clamp(const T X, const T MinValue, const T MaxValue)
	{
		return Max(Min(X, MaxValue), MinValue);
	}

	/** A의 절댓값을 구합니다. */
	template <typename T>
	[[nodiscard]] static FORCEINLINE constexpr T Abs(const T A)
	{
		return A < T(0) ? -A : A;
	}

    /** Returns 1, 0, or -1 depending on relation of T to 0 */
    template< class T >
    static constexpr FORCEINLINE T Sign(const T A)
	{
	    return (A > (T)0) ? (T)1 : ((A < (T)0) ? (T)-1 : (T)0);
	}

	/** A의 제곱을 구합니다. */
	template <typename T>
	[[nodiscard]] static FORCEINLINE constexpr T Pow(const T A)
	{
		return A * A;
	}

    static FORCEINLINE float Pow(float A, float B) { return powf(A,B); }

	// A의 제곱근을 구합니다.
	[[nodiscard]] static FORCEINLINE float Sqrt(float A) { return sqrtf(A); }
	[[nodiscard]] static FORCEINLINE double Sqrt(double A) { return sqrt(A); }

	/** A의 역제곱근을 구합니다. */
	[[nodiscard]] static FORCEINLINE float InvSqrt(float A) { return 1.0f / sqrtf(A); }
	[[nodiscard]] static FORCEINLINE double InvSqrt(double A) { return 1.0 / sqrt(A); }

	/** A와 B를 Alpha값에 따라 선형으로 보간합니다. */
	template <typename T>
	[[nodiscard]] static FORCEINLINE constexpr T Lerp(const T& A, const T& B, float Alpha)
	{
        Alpha = Clamp(Alpha, 0.0f, 1.0f);
		return static_cast<T>((A * (1.0f - Alpha)) + (B * Alpha));
	}

	/** A와 B를 Alpha값에 따라 선형으로 보간합니다. */
	template <typename T>
	[[nodiscard]] static FORCEINLINE constexpr T Lerp(const T& A, const T& B, double Alpha)
	{
		return static_cast<T>((A * (1.0 - Alpha)) + (B * Alpha));
	}

	template <typename T>
	[[nodiscard]] static FORCEINLINE constexpr auto RadiansToDegrees(const T& RadVal) -> decltype(RadVal * (180.0f / PI))
	{
		return RadVal * (180.0f / PI);
	}

	[[nodiscard]] static FORCEINLINE constexpr float RadiansToDegrees(float RadVal)
	{
		return RadVal * (180.0f / PI);
	}

	[[nodiscard]] static FORCEINLINE constexpr double RadiansToDegrees(double RadVal)
	{
		return RadVal * (180.0 / PI_DOUBLE);
	}

	template <typename T>
	[[nodiscard]] static FORCEINLINE constexpr auto DegreesToRadians(const T& DegVal) -> decltype(DegVal * (PI / 180.0f))
	{
		return DegVal * (PI / 180.0f);
	}

	[[nodiscard]] static FORCEINLINE constexpr float DegreesToRadians(float DegVal)
	{
		return DegVal * (PI / 180.0f);
	}

	[[nodiscard]] static FORCEINLINE constexpr double DegreesToRadians(double DegVal)
	{
		return DegVal * (PI_DOUBLE / 180.0);
	}

    static FORCEINLINE float Loge(float Value) { return logf(Value); }
    static FORCEINLINE double Loge(double Value) { return log(Value); }

    static FORCEINLINE float LogX(float Base, float Value) { return Loge(Value) / Loge(Base); }
    static FORCEINLINE double LogX(double Base, double Value) { return Loge(Value) / Loge(Base); }

    // 1.0 / Loge(2) = 1.4426950f
    static FORCEINLINE float Log2(float Value) { return Loge(Value) * std::numbers::log2e_v<float>; }
    // 1.0 / Loge(2) = 1.442695040888963387
    static FORCEINLINE double Log2(double Value) { return Loge(Value) * std::numbers::log2e; }

    [[nodiscard]] static FORCEINLINE bool IsNearlyEqual(float A, float B, float ErrorTolerance = KINDA_SMALL_NUMBER)
	{
	    return Abs<float>(A - B) <= ErrorTolerance;
	}

    [[nodiscard]] static FORCEINLINE bool IsNearlyEqual(double A, double B, double ErrorTolerance = SMALL_NUMBER)
	{
	    return Abs<double>(A - B) <= ErrorTolerance;
	}

    [[nodiscard]] static FORCEINLINE bool IsNearlyZero(float Value, float ErrorTolerance = KINDA_SMALL_NUMBER)
	{
	    return Abs<float>(Value) <= ErrorTolerance;
	}

    [[nodiscard]] static FORCEINLINE bool IsNearlyZero(double Value, double ErrorTolerance = SMALL_NUMBER)
	{
	    return Abs<double>(Value) <= ErrorTolerance;
	}

	[[nodiscard]] static FORCEINLINE double Cos(double RadVal)
	{
		return cos(RadVal);
	}

	[[nodiscard]] static FORCEINLINE float Cos(float RadVal)
	{
		return cosf(RadVal);
	}

	[[nodiscard]] static FORCEINLINE double Sin(double RadVal)
	{
		return sin(RadVal);
	}

	[[nodiscard]] static FORCEINLINE float Sin(float RadVal)
	{
		return sinf(RadVal);
	}

	[[nodiscard]] static FORCEINLINE double Tan(double RadVal)
	{
		return tan(RadVal);
	}

	[[nodiscard]] static FORCEINLINE float Tan(float RadVal)
	{
		return tanf(RadVal);
	}

	[[nodiscard]] static FORCEINLINE double Acos(double Value)
	{
		return acos(Value);
	}

	[[nodiscard]] static FORCEINLINE float Acos(float Value)
	{
		return acosf(Value);
	}

	[[nodiscard]] static FORCEINLINE double Asin(double Value)
	{
		return asin(Value);
	}

	[[nodiscard]] static FORCEINLINE float Asin(float Value)
	{
		return asinf(Value);
	}

	[[nodiscard]] static FORCEINLINE double Atan(double Value)
	{
		return atan(Value);
	}

	[[nodiscard]] static FORCEINLINE float Atan(float Value)
	{
		return atanf(Value);
	}

	[[nodiscard]] static FORCEINLINE double Atan2(double Y, double X)
	{
		return atan2(Y, X);
	}

	[[nodiscard]] static FORCEINLINE float Atan2(float Y, float X)
	{
		return atan2f(Y, X);
	}

	template <typename T>
	[[nodiscard]] static FORCEINLINE T Square(T Value)
	{
		return Value * Value;
	}

	template <typename T>
	[[nodiscard]] static FORCEINLINE int32 CeilToInt(T Value)
	{
		return static_cast<int32>(ceil(Value));
	}

	/** float 타입을 위한 특수화된 버전 */
	[[nodiscard]] static FORCEINLINE int32 CeilToInt(float Value)
	{
		return static_cast<int32>(ceilf(Value));
	}

	/** double 타입을 위한 특수화된 버전 */
	[[nodiscard]] static FORCEINLINE int32 CeilToInt(double Value)
	{
		return static_cast<int32>(ceil(Value));
	}

	static FORCEINLINE void SinCos(float* ScalarSin, float* ScalarCos, float Value)
	{
		*ScalarSin = sinf(Value);
		*ScalarCos = cosf(Value);
	}

	static FORCEINLINE void SinCos(double* ScalarSin, double* ScalarCos, double Value)
	{
		*ScalarSin = sin(Value);
		*ScalarCos = cos(Value);
	}

	[[nodiscard]] static FORCEINLINE float UnwindDegrees(float A)
	{
		while (A > 180.0f)
		{
			A -= 360.0f;
		}
		while (A < -180.0f)
		{
			A += 360.0f;
		}
		return A;
	}

    /**
     * Converts a float to an integer with truncation towards zero.
     * @param F		Floating point value to convert
     * @return		Truncated integer.
     */
    static constexpr FORCEINLINE int32 Trunc( float F )
	{
	    return static_cast<int32>(F);
	}

    /**
     * Converts a float to an integer value with truncation towards zero.
     * @param F		Floating point value to convert
     * @return		Truncated integer value.
     */
    static constexpr FORCEINLINE float TruncFloat( float F )
	{
	    return static_cast<float>(Trunc(F));
	}

    /**
     * Converts a float to a less or equal integer.
     * @param F		Floating point value to convert
     * @return		An integer less or equal to 'F'.
     */
    static FORCEINLINE int32 Floor( float F )
	{
	    return Trunc(floorf(F));
	}

    // Math Operations

    /** Returns highest of 3 values */
    template< class T > 
    static FORCEINLINE T Max3( const T A, const T B, const T C )
	{
	    return Max ( Max( A, B ), C );
	}

    /** Returns lowest of 3 values */
    template< class T > 
    static FORCEINLINE T Min3( const T A, const T B, const T C )
	{
	    return Min ( Min( A, B ), C );
	}

    [[nodiscard]] static FORCEINLINE int RandRange(int Min, int Max)
    {
        return Min + (std::rand() % ((Max - Min) + 1));
    }

    /** 실수형 범위 내에서 난수 반환 */
    [[nodiscard]] static FORCEINLINE float RandRange(float Min, float Max)
    {
        float Ratio = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
        return Min + (Max - Min) * Ratio;
    }

    // FMath 내부에 추가
    static float InterpEaseOut(float A, float B, float Alpha, float Exp)
    {
        Alpha = FMath::Clamp(Alpha, 0.0f, 1.0f);
        return A + (B - A) * FMath::Pow(1 - FMath::Pow(1 - Alpha, Exp));
    }

    [[nodiscard]] static float Fmod(float X, float Y)
	{
	    const float AbsY = FMath::Abs(Y);
	    if (AbsY <= SMALL_NUMBER)
	    {
	        return 0.0;
	    }

	    return fmodf(X, Y);
	}
};
