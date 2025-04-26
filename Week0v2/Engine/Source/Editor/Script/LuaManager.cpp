#include "LuaManager.h"

#include "GameFramework/Actor.h"
#include "Math/Vector.h"


void FLuaManager::Initialize()
{
    try {
        Lua.open_libraries(
            sol::lib::base,     // 필수: print, type, pairs, ipairs 등 기본 함수
            sol::lib::package,  // 모듈 시스템 사용 (require) 시 필요
            sol::lib::string,   // 문자열 조작 함수
            sol::lib::math,     // 수학 함수 (math.random, math.sin 등)
            sol::lib::table     // 테이블 조작 함수
            // 필요에 따라 sol::lib::os, sol::lib::io, sol::lib::debug 등을 추가
        );
        std::cout << "[LuaManager] Standard libraries opened." << std::endl;
    } catch (const sol::error& e) {
        std::cerr << "[LuaManager] Error opening standard libraries: " << e.what() << std::endl;
        // 초기화 실패 처리
        return; // 또는 false 반환
    }

    try {
        // --- 수학/기본 타입 등록 ---
        Lua.new_usertype<FVector>("Vector", // 예시: FVector 등록
            sol::constructors<FVector(), FVector(float, float, float)>(),
            "x", &FVector::x,
            "y", &FVector::y,
            "z", &FVector::z,
            sol::meta_function::addition, [](const FVector& a, const FVector& b) { return a + b; },
            sol::meta_function::multiplication, [](const FVector& v, float f) { return v * f; }
            // ... 기타 필요한 연산자 및 멤버 함수 ...
        );
        // ... FRotator, FTransform 등 다른 기본 타입 등록 ...

        // --- 엔진 코어 타입 등록 (상속 관계 주의) ---
        // 부모 클래스를 먼저 등록해야 합니다!
        // Lua.new_usertype<UObject>("Object",
        //     
        //     /* ... UObject 멤버 ... */);

        Lua.new_usertype<UObject>("Object");
        
        Lua.new_usertype<AActor>("Actor",
            sol::bases<UObject>(), // 상속 관계 명시
            sol::constructors<AActor()>(), 
            //"ActorLabel", sol::property(&AActor::GetActorLabel, &AActor::SetActorLabel),
            "GetActorLocation", &AActor::GetActorLocation, // 멤버 함수
            "SetActorLocation", &AActor::SetActorLocation
            // ... 기타 필요한 액터 멤버/함수 ...
        );
        
        // Lua.new_usertype<UActorComponent>("ActorComponent",
        //     sol::bases<UObject>(),
        //     "GetOwner", &UActorComponent::GetOwner // 예시 멤버 함수
        //     // ... 컴포넌트 공통 멤버/함수 ...
        // );
        //
        // Lua.new_usertype<UPrimitiveComponent>("PrimitiveComponent",
        //      sol::bases<UActorComponent>(), // 상속
        //      // ... PrimitiveComponent 멤버 (예: Overlap 함수 바인딩 콜백 설정?)
        // );
        //
        // Lua.new_usertype<UBoxComponent>("BoxComponent",
        //      sol::bases<UPrimitiveComponent>() // 상속
        //      // ... BoxComponent 고유 멤버 (BoxExtent 등)
        // );
        // // ... USphereComponent, UCapsuleComponent 등 다른 컴포넌트 등록 ...

        std::cout << "[LuaManager] Core C++ types registered." << std::endl;

    } catch (const sol::error& e) {
        std::cerr << "[LuaManager] Error registering usertypes: " << e.what() << std::endl;
        // 초기화 실패 처리
        return;
    }

    try {
        Lua.set_function("LogMessage", [](const std::string& message) {
            // 엔진의 로그 시스템을 사용하는 것이 좋음 (예: UE_LOG)
            std::cout << "[Lua Log] " << message << std::endl;
        });

        // Lua.set_function("SpawnActor", [](const std::string& actorClassName, const FVector& location) -> AActor* {
        //     // 엔진의 액터 스폰 로직 호출
        //     // 예: return GWorld->SpawnActor<AActor>(FindClass(actorClassName), location, FRotator::ZeroRotator);
        //     std::cout << "[Lua Call] SpawnActor called for " << actorClassName << " at (" << location.x << ", ...)" << std::endl;
        //     return nullptr; // 실제 구현 필요
        // });
        std::cout << "[LuaManager] Global C++ functions registered." << std::endl;
    } catch (const sol::error& e) {
        std::cerr << "[LuaManager] Error registering global functions: " << e.what() << std::endl;
        // 초기화 실패 처리
        return;
    }
}

void FLuaManager::Release()
{
}
