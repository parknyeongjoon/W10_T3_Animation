#pragma once

#include <cstdint> // for uint32, uint64
#include <cstdlib> // for rand(), srand()
#include <ctime>   // for time()
#include <functional> // for std::hash (optional for GetTypeHash)

#include "HAL/PlatformType.h"

class FArchive;
// 전방 선언 (FString이 다른 곳에 정의되어 있을 경우)
/**
 * 간단한 전역 고유 식별자 (GUID) 구조체.
 * 주의: 이 구현은 암호학적으로 안전하지 않으며,
 * 프로그램 실행 간 또는 다른 머신 간의 전역 고유성을 엄격하게 보장하지 않습니다.
 * 주로 단일 게임 세션 내에서 객체를 고유하게 식별하는 데 사용됩니다.
 */
struct FGuid
{
public:
    uint32 A = 0;
    uint32 B = 0;
    uint32 C = 0;
    uint32 D = 0;

    // 기본 생성자 (모든 값을 0으로 초기화 - "Invalid" GUID)
    FGuid() = default;

    // 특정 값으로 초기화하는 생성자 (필요한 경우)
    FGuid(uint32 InA, uint32 InB, uint32 InC, uint32 InD)
        : A(InA), B(InB), C(InC), D(InD)
    {}

    /** 새로운 (세션 내에서 고유할 가능성이 높은) GUID를 생성합니다. */
    static FGuid NewGuid()
    {
        // 간단한 난수 생성을 위해 srand/rand 사용 (더 나은 방법은 <random> 사용)
        InitializeGenerator(); // 필요시 난수 생성기 초기화

        static uint64 Counter = 0; // 세션 내 고유성을 위한 간단한 카운터
        Counter++;

        FGuid NewGuid;
        // 카운터와 난수를 조합하여 생성
        NewGuid.A = static_cast<uint32>(Counter >> 32); // 카운터 상위 비트
        NewGuid.B = static_cast<uint32>(Counter & 0xFFFFFFFF); // 카운터 하위 비트
        NewGuid.C = static_cast<uint32>(rand()) ^ (static_cast<uint32>(rand()) << 16);
        NewGuid.D = static_cast<uint32>(rand()) ^ (static_cast<uint32>(rand()) << 16);

        // 0 값 GUID가 생성될 확률을 매우 낮추기 위해 D가 0이면 다시 시도 (매우 드문 경우)
        if (NewGuid.A == 0 && NewGuid.B == 0 && NewGuid.C == 0 && NewGuid.D == 0)
        {
             NewGuid.D = 1; // 또는 다른 비-0 값
        }

        return NewGuid;
    }

    /** GUID가 유효한지 (모든 필드가 0이 아닌지) 확인합니다. */
    bool IsValid() const
    {
        return A != 0 || B != 0 || C != 0 || D != 0;
    }

    /** 비교 연산자 */
    bool operator==(const FGuid& Other) const
    {
        return A == Other.A && B == Other.B && C == Other.C && D == Other.D;
    }

    bool operator!=(const FGuid& Other) const
    {
        return !(*this == Other);
    }

    // TMap 등에서 사용하기 위한 정렬 연산자 (선택 사항)
    bool operator<(const FGuid& Other) const
    {
        if (A != Other.A) return A < Other.A;
        if (B != Other.B) return B < Other.B;
        if (C != Other.C) return C < Other.C;
        return D < Other.D;
    }



private:
    /** 난수 생성기를 초기화합니다 (최초 한 번만 실행). */
    static void InitializeGenerator()
    {
        static bool bInitialized = false;
        if (!bInitialized)
        {
            // 현재 시간을 시드로 사용
            srand(static_cast<unsigned int>(time(nullptr)));
            bInitialized = true;
        }
    }

    //TODO 순환참조 이슈로 작동안됨
    /** FArchive를 사용하여 데이터를 직렬화합니다. */
    void Serialize(FArchive& Ar) const;// 쓰는 작업이므로 const로 선언

    /** FArchive를 사용하여 데이터를 역직렬화합니다. */
    void Deserialize(FArchive& Ar); // 읽는 작업이므로 비-const

};

// --- 전역 함수 정의 ---

/** TMap 등에서 사용하기 위한 해시 함수 (Unreal Engine 스타일) */
inline uint32 GetTypeHash(const FGuid& Guid)
{
    // 간단한 해시 조합 (더 나은 방법은 존재함)
    uint32 Hash = Guid.A;
    Hash = Hash ^ (Guid.B + 0x9e3779b9 + (Hash << 6) + (Hash >> 2)); // Boost::hash_combine 유사 패턴
    Hash = Hash ^ (Guid.C + 0x9e3779b9 + (Hash << 6) + (Hash >> 2));
    Hash = Hash ^ (Guid.D + 0x9e3779b9 + (Hash << 6) + (Hash >> 2));
    return Hash;
}


// FString ToString() 구현 예시 (엔진의 FString.h 및 FString::Printf 필요)

// #include "Container/String.h" // 실제 FString 헤더 경로로 변경해야 함
//
// inline FString FGuid::ToString() const
// {
//     // 예시 포맷: 8-8-8-8 (실제 GUID 포맷과 다름)
//     return FString::Printf(TEXT("%08X-%08X-%08X-%08X"), A, B, C, D);
// }


// std::string ToStringStd() 구현 예시 (표준 라이브러리 사용)
/*
#include <string>
#include <sstream>
#include <iomanip>

inline std::string FGuid::ToStringStd() const
{
    std::stringstream ss;
    ss << std::hex << std::uppercase << std::setfill('0')
       << std::setw(8) << A << "-"
       << std::setw(8) << B << "-"
       << std::setw(8) << C << "-"
       << std::setw(8) << D;
    return ss.str();
}
*/
namespace std {
    template <> // 템플릿 특수화임을 명시
    struct hash<FGuid>
    {
        // operator()는 const FGuid&를 받고 size_t를 반환해야 함
        size_t operator()(const FGuid& guid) const noexcept // noexcept 추가 권장
        {
            // 기존에 만든 GetTypeHash 함수를 호출하여 해시 값 계산
            // GetTypeHash가 uint32를 반환하면 size_t로 캐스팅
            return static_cast<size_t>(GetTypeHash(guid));
        }
    };
} // namespace std