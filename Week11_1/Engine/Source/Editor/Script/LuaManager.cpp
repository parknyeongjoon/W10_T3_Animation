#include "LuaManager.h"

#include "GameFramework/Actor.h"
#include "Math/Vector.h"

#include <iostream> // 오류 로깅용 (엔진 로깅으로 교체)
#include <filesystem> // 경로 조작용 (C++17) - 구 버전 C++ 또는 엔진 특정 기능 사용 시 조정

#include "Components/Mesh/StaticMesh.h"
#include "Engine/Level.h"
#include "LuaTypes/LuaUserTypes.h"
#include "LuaUtils/LuaStub.h"

#include "sol/sol.hpp"

// AActor 전방 선언 또는 헤더 포함 (BindAActor가 다른 곳에 정의된 경우)
// class AActor;
// void BindAActor(sol::state& lua);

// 싱글톤을 위한 정적 인스턴스 및 플래그 정의
std::unique_ptr<FLuaManager> FLuaManager::Instance = nullptr;
std::once_flag FLuaManager::InitInstanceFlag;

// --- 싱글톤 구현 ---
FLuaManager& FLuaManager::Get()
{
    // 스레드 안전 초기화
    std::call_once(InitInstanceFlag, []() {
        Instance.reset(new FLuaManager());
        // 여기서 Initialize를 호출하거나, 인스턴스 획득 후 명시적 호출 필요
    });
    return *Instance;
}

// --- 초기화 및 종료 ---
void FLuaManager::Initialize()
{
    if (bInitialized) return;
    bInitialized = true;
    
    std::cout << "FLuaManager 초기화 중..." << std::endl;
    try {
        // 표준 Lua 라이브러리 열기
        LuaState.open_libraries(
            sol::lib::base,       // Lua를 사용하기 위한 기본 라이브러리 (print, type, pcall 등)
            sol::lib::package,    // 모듈 로딩(require) 및 패키지 관리 기능
            sol::lib::coroutine,  // 코루틴 생성 및 관리 기능 (yield, resume 등)
            sol::lib::string,     // 문자열 검색, 치환, 포맷팅 등 문자열 처리 기능
            sol::lib::math,       // 수학 함수 (sin, random, pi 등) 및 상수 제공
            sol::lib::table,      // 테이블(배열/딕셔너리) 생성 및 조작 기능 (insert, sort 등)
            sol::lib::io,         // 파일 읽기/쓰기 등 입출력 관련 기능
            sol::lib::os,         // 운영체제 관련 기능 (시간, 날짜, 파일 시스템 접근 등)
            sol::lib::debug,      // 디버깅 및 introspection 기능 (traceback, getinfo 등)
            sol::lib::bit32,      // 32비트 정수 대상 비트 연산 기능 (Lua 5.2 이상)
            // sol::lib::jit,        // LuaJIT의 JIT 컴파일러 제어 기능 (LuaJIT 전용)
            // sol::lib::ffi,        // 외부 C 함수 및 데이터 구조 접근 기능 (LuaJIT 전용)
            sol::lib::utf8        // UTF-8 인코딩 문자열 처리 기능 (Lua 5.3 이상)
        );
        LuaState["Log"] = [](const std::string& message)
        {
            std::cout << "[LuaLog] " << message << std::endl;
        };

        LuaState.set_function("PrintObject", [](const sol::object& obj)
        {
            std::cout << LuaObjToString(obj) << std::endl;
        });
        
        // Lua 스크립트에 필요한 핵심 C++ 타입 바인딩
        BindCoreTypes();

        std::cout << "FLuaManager 초기화 완료." << std::endl;

    } catch (const sol::error& e) {
        std::cerr << "치명적 오류: Lua 초기화 중 문제 발생: " << e.what() << std::endl;
        // 치명적 오류 적절히 처리 (예: 예외 던지기, 프로그램 종료)
    }
}

void FLuaManager::Shutdown()
{
    std::cout << "FLuaManager 종료 중..." << std::endl;
    // 스크립트 캐시 비우기. sol::table 객체는 Lua에 의해 가비지 컬렉션됨.
    LoadedScriptTables.clear();
    // Lua 상태 (`sol::state Lua;`)는 FLuaManager 인스턴스가 파괴될 때
    // RAII 덕분에 자동으로 정리됨.
    std::cout << "FLuaManager 종료됨." << std::endl;
}

void FLuaManager::BeginPlay()
{
    //파이 모드 들어갈때 캐쉬테이블 초기화
    LoadedScriptTables.clear();
}

//// --- 스크립트 관리 ---
sol::table FLuaManager::GetOrLoadScriptTable(const FString& ScriptPath) // FString이 std::string이라고 가정
{
    // 1. 캐시 확인 (동일)
    auto it = LoadedScriptTables.find(ScriptPath);
    if (it != LoadedScriptTables.end())
    {
        // 캐시 히트
        // std::cout << "스크립트 캐시 히트: " << ScriptPath << std::endl;
        return it->second;
    }

    // 2. 캐시 미스 - 스크립트 로드 및 실행
    // std::cout << "스크립트 캐시 미스: " << ScriptPath << ". 로딩 시작..." << std::endl;
    FString FullPath = ScriptsBasePath + ScriptPath; // 전체 경로 구성

    try {
        // 파일 존재 여부 확인 (동일)
        if (!std::filesystem::exists(FullPath)) { // <filesystem> 필요, FullPath 직접 사용
             std::cerr << "오류: 스크립트 파일을 찾을 수 없습니다: " << FullPath << std::endl;
             return sol::lua_nil; // 유효하지 않은 테이블 반환
        }

        // --- load_file 사용 방식으로 변경 ---

        // 단계 1: 스크립트 파일을 로드하고 컴파일합니다. (실행은 아직 안 함)
        sol::load_result loadResult = LuaState.load_file(*FullPath); // FullPath 직접 사용

        // 단계 2: 로딩 및 컴파일 성공 여부 확인
        if (!loadResult.valid()) {
            // 로딩 또는 컴파일 중 구문 오류 등 발생
            sol::error err = loadResult; // 오류 정보 추출
            std::cerr << "Error: Script file loading/compilation failed '" << FullPath << "': " << err.what() << std::endl;
            return sol::lua_nil; // 유효하지 않은 테이블 반환
        }

        // 단계 3: 로딩/컴파일 성공 시, 컴파일된 함수(청크)를 얻음
        sol::protected_function scriptChunk = loadResult; // load_file의 결과는 실행 가능한 함수(청크)

        // 단계 4: 컴파일된 함수(청크)를 안전하게 실행 (이 때 스크립트 코드가 실제로 실행됨)
        sol::protected_function_result executionResult = scriptChunk(); // 청크 실행

        // 단계 5: 청크 실행 성공 여부 확인 (런타임 오류 확인)
        if (!executionResult.valid()) {
            // 스크립트 실행 중 런타임 오류 발생
            sol::error err = executionResult;
            std::cerr << "Error: Script chunk execution failed  '" << FullPath << "': " << err.what() << std::endl;
            return sol::lua_nil;
        }

        // 단계 6: 실행 결과에서 반환된 값을 가져오고 테이블인지 확인
        // executionResult.get<T>() 로 반환값 가져오기
        sol::object returned_value = executionResult.get<sol::object>(); // get<>으로 실제 반환값 추출
        if (returned_value.is<sol::table>())
        {
            // 단계 7: 테이블이 맞으면 캐시에 저장하고 반환
            sol::table scriptTable = returned_value.as<sol::table>();
            std::cout << "Script loaded, executed, and cached successfully: " << ScriptPath << std::endl; // ScriptPath 직접 사용
            LoadedScriptTables[ScriptPath] = scriptTable; // 캐시에 저장
            return scriptTable;
        }
        else
        {
            // 단계 8: 테이블이 아니면 오류 처리
             std::cerr << "Error: Script " << ScriptPath << " did not return a table after execution." << std::endl;
             return sol::lua_nil; // 유효하지 않은 테이블 반환
        }

        // --- 변경 완료 ---

    } catch (const sol::error& e) {
        // Lua 상호작용 중 예외 처리 (동일)
        std::cerr << "Lua Exception occurred (while processing script) '" << FullPath << "': " << e.what() << std::endl;
        return sol::lua_nil;
    } catch (const std::exception& e) {
        // 표준 예외 처리 (동일)
        std::cerr << "Standard exception occurred (during script processing) '" << FullPath << "': " << e.what() << std::endl;
        return sol::lua_nil;
    }
}

// --- Lua 상태 접근 ---
sol::state& FLuaManager::GetLuaState()
{
    return LuaState;
}


// --- 헬퍼 함수 ---
void FLuaManager::BindCoreTypes()
{
    sol::table Ns = LuaState.create_named_table("Engine");

     //Math Types
     LuaTypes::FBindLua<FColor>::Bind(Ns);
     LuaTypes::FBindLua<FLinearColor>::Bind(Ns);
     LuaTypes::FBindLua<FVector>::Bind(Ns);
     LuaTypes::FBindLua<FVector2D>::Bind(Ns);
     LuaTypes::FBindLua<FVector4>::Bind(Ns);
     LuaTypes::FBindLua<FRotator>::Bind(Ns);
     LuaTypes::FBindLua<FQuat>::Bind(Ns);
     LuaTypes::FBindLua<FMatrix>::Bind(Ns);

    BindFStringToLua(Ns);

    
    // Object Types
    // 부모 부터 바인딩 해야함
    //LuaTypes::FBindLua<ALuaActor>::Bind(Ns);
    // LuaTypes::FBindLua<UObject>::Bind(Ns);
    // LuaTypes::FBindLua<AActor>::Bind(Ns);
    //
    // LuaTypes::FBindLua<UActorComponent>::Bind(Ns);
    // LuaTypes::FBindLua<USceneComponent>::Bind(Ns);
    // LuaTypes::FBindLua<UStaticMeshComponent>::Bind(Ns);

    for (const auto& [name, meta]: UClass::GetClassMap())
    {
        if (meta->IsChildOf<AActor>())
        {
            meta->BindPropertiesToLua(LuaState);
        }
    }
    for (const auto& [name, meta]: UClass::GetClassMap())
    {
        if (meta->IsChildOf<UActorComponent>())
        {
            meta->BindPropertiesToLua(LuaState);
        }
    }
    // UFUNCTION으로 안되는 케이스들 별도로 등록.
    sol::usertype<UActorComponent> ActorComponentTypeTable = UActorComponent::GetLuaUserType(LuaState);
    ActorComponentTypeTable["GetOwner"] = &UActorComponent::GetOwner;


    generateStubs(LuaState);
}


void FLuaManager::BindFStringToLua(sol::table& lua)
{
        // ESearchCase 열거형 바인딩
    lua.new_enum("ESearchCase",
        "CaseSensitive", ESearchCase::CaseSensitive,
        "IgnoreCase", ESearchCase::IgnoreCase
    );

    // ESearchDir 열거형 바인딩
    lua.new_enum("ESearchDir",
        "FromStart", ESearchDir::FromStart,
        "FromEnd", ESearchDir::FromEnd
    );

    // FString 클래스 바인딩
    lua.new_usertype<FString>("FString",
        // 생성자 바인딩
        sol::constructors<
            FString(),                                  // 기본 생성자
            FString(const FString&),                    // 복사 생성자
#if USE_WIDECHAR
            FString(const wchar_t*),                    // Wide C 문자열로부터 생성
            FString(const char*),                       // Ansi C 문자열로부터 생성 (내부 변환)
            FString(const std::wstring&),               // std::wstring으로부터 생성
            FString(const std::string&)                 // std::string으로부터 생성 (내부 변환)
#else
            FString(const char*),                       // Ansi C 문자열로부터 생성
            FString(const std::string&)                 // std::string으로부터 생성
            // Wide 문자열 생성자는 explicit이므로, 필요시 별도 팩토리 함수로 노출 가능
            // 예: sol::factories([](const wchar_t* ws){ return FString(ws); })
#endif
        >(),

        // 멤버 함수 및 프로퍼티 바인딩
        "Len", &FString::Len, // 문자열 길이
        "IsEmpty", &FString::IsEmpty, // 비어있는지 확인
        "Left", &FString::Left, // 왼쪽 부분 문자열
        "RightChop", &FString::RightChop, // 오른쪽 부분 제거
        "Empty", &FString::Empty, // 문자열 비우기
        "Reserve", &FString::Reserve, // 메모리 예약
        "Resize", &FString::Resize,   // 크기 조절 (주의: 내용이 변경될 수 있음)

        // 비교 및 검색
        "Equals", &FString::Equals,
        "Contains", &FString::Contains,
        "Find", &FString::Find,

        // 대소문자 변환
        "ToUpper", sol::resolve<FString() const &>(&FString::ToUpper), // 원본 불변 버전
        "ToLower", sol::resolve<FString() const &>(&FString::ToLower), // 원본 불변 버전
        "ToUpperInline", &FString::ToUpperInline, // 내부 수정 버전
        "ToLowerInline", &FString::ToLowerInline, // 내부 수정 버전

        // 변환 함수
        "ToInt", &FString::ToInt,
        "ToFloat", &FString::ToFloat,
        "ToBool", &FString::ToBool,
        "toPathString", [](const FString& self) { // std::filesystem::path 암시적 변환 대신 명시적 함수
            std::filesystem::path p = self; // operator PathType() 호출
            #if defined(_MSC_VER) && defined(USE_WIDECHAR) && USE_WIDECHAR // Windows + WideChar 환경 대응
                 // Windows에서 filesystem::path::string()은 UTF-8 멀티바이트 문자열을 반환함
                 return p.string();
            #elif defined(USE_WIDECHAR) && USE_WIDECHAR // Non-Windows + WideChar
                 // Non-Windows 환경에서 wstring을 UTF8로 변환하여 반환
                 return WideStringToUTF8(p.wstring());
            #else // ANSI Char 환경
                return p.string(); // ANSI 환경에서는 그냥 string() 사용
            #endif
        },


        // 연산자 오버로딩 (메타메소드)
        sol::meta_function::addition, sol::resolve<FString(const FString&, const FString&)>(&operator+), // '+' 연산자 (문자열 덧셈)
        sol::meta_function::concatenation, sol::resolve<FString(const FString&, const FString&)>(&operator+), // '..' 연산자 (Lua의 문자열 연결)
        sol::meta_function::equal_to, sol::resolve<bool(const FString&) const>(&FString::operator==), // '==' 연산자
        sol::meta_function::to_string, [](const FString& self) { // Lua의 print()나 tostring()에서 사용될 표현
#if USE_WIDECHAR
            // WideChar 문자열을 Lua가 이해하는 UTF-8 문자열로 변환
             return self.ToAnsiString(); // ToAnsiString이 UTF-8을 반환한다고 가정
#else
            // AnsiChar 문자열은 그대로 사용 (operator*() 활용)
            return std::string(*self); // const char*를 std::string으로 변환하여 반환 (sol2가 관리)
#endif
        },

         // operator+= (메소드로 노출하는 것이 더 명확할 수 있음)
        "Append", sol::resolve<FString&(const FString&)>(&FString::operator+=), // += 연산자를 'Append' 메소드로 노출


        // 정적 멤버 함수 바인딩
        // 주의: FromInt 템플릿은 구체적인 타입으로 캐스팅하여 오버로드로 제공
        "FromInt", sol::overload(
            static_cast<FString(*)(int)>(&FString::FromInt<int>),
            static_cast<FString(*)(long)>(&FString::FromInt<long>),
            static_cast<FString(*)(long long)>(&FString::FromInt<long long>),
            static_cast<FString(*)(unsigned int)>(&FString::FromInt<unsigned int>),
            static_cast<FString(*)(unsigned long)>(&FString::FromInt<unsigned long>),
            static_cast<FString(*)(unsigned long long)>(&FString::FromInt<unsigned long long>)
            // 필요한 다른 정수 타입 추가...
        ),
        "FromFloat", static_cast<FString(*)(float)>(&FString::FromInt<float>), // FromInt를 float용으로 사용
        "FromDouble", static_cast<FString(*)(double)>(&FString::FromInt<double>), // FromInt를 double용으로 사용
        "SanitizeFloat", &FString::SanitizeFloat
        // "Printf", &FString::Printf // 가변 인자 함수 바인딩 (주의 필요 - 아래 설명 참조)
        // Printf 를 위한 대안 (Lua 측에서 처리)
        // Lua 스크립트에서: local formattedString = string.format("Hello %s %d", fstringInstance:ToString(), 123)
    );
}

std::string FLuaManager::LuaObjToString(const sol::object& obj, int depth, bool showHidden) {
    if (obj.get_type() == sol::type::nil) {
       return "nil";
    } else if (obj.is<std::string>()) {
        return "\"" + obj.as<std::string>() + "\"";
    } else if (obj.is<int>()) {
        return std::to_string(obj.as<int>());
    } else if (obj.is<double>()) {
        return std::to_string(obj.as<double>());
    } else if (obj.is<bool>()) {
        return obj.as<bool>() ? "true" : "false";
    } else if (obj.get_type() == sol::type::table) {
        std::string result = "{";
        sol::table tbl = obj;
        bool first = true;
        for (auto& kv : tbl) {
            if (!showHidden && kv.first.as<std::string>().starts_with("__"))
                continue;
            if (!first) result += ", ";
            first = false;
            result += LuaObjToString(kv.first, depth + 1, showHidden) + " : " + LuaObjToString(kv.second, depth + 1, showHidden);
        }
        result += "}";
        return result;
    } else  if (obj.get_type() == sol::type::userdata) {
        sol::table tbl = obj;
        sol::table metatable = tbl[sol::metatable_key];
        std::string type = metatable["__type"]["name"];
        if (obj.is<FVector>())
        {
            FVector vec = obj.as<FVector>();
            std::string result = "[FVector](";
            result += vec.X; result += ",";
            result += vec.Y; result += ",";
            result += vec.Z; result += ")";
            return result;
        }
        else if (obj.is<FString>())
        {
            std::string result = "[FString]\"";
            result += GetData(obj.as<FString>());
            result += "\"";
            return result;
        }
        else if (obj.is<FRotator>())
        {
            FRotator rot = obj.as<FRotator>();
            std::string result = "[FRotator](";
            result += rot.Pitch; result += ",";
            result += rot.Yaw; result += ",";
            result += rot.Roll; result += ")";
            return result;
        }
        else
        {
            std::string result = "[";
            result += metatable["__type"]["name"];
            result += "]:";
            result += LuaObjToString(metatable, depth + 1, false);
            return result;
        }
    } else if (obj.get_type() == sol::type::function) {
        return "[function]";
    } else {
        return "[unknown type]";
    }
}