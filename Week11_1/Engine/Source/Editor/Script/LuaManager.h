#pragma once
#include <sol/sol.hpp>
#include <string> // 또는 엔진의 문자열 타입 (예: FString)
#include <unordered_map>
#include <memory> // std::unique_ptr (싱글톤용)
#include <mutex>  // std::once_flag (싱글톤용)

#include "Container/String.h"

class FLuaManager
{
public:
    // --- 싱글톤 접근 ---
    // Lua 관리자의 단일 인스턴스를 가져옵니다.
    static FLuaManager& Get();

    // 복사/이동 생성자 및 대입 연산자 삭제
    FLuaManager(const FLuaManager&) = delete;
    FLuaManager& operator=(const FLuaManager&) = delete;
    FLuaManager(FLuaManager&&) = delete;
    FLuaManager& operator=(FLuaManager&&) = delete;

    // --- 초기화 및 종료 ---
    // Lua 상태(state) 및 라이브러리를 초기화합니다.
    void Initialize();
    // 리소스를 정리하고, 잠재적으로 Lua 상태를 닫습니다 (RAII가 처리).
    void Shutdown();

    void BeginPlay();

    // --- 스크립트 관리 ---
    /**
     * @brief 주어진 스크립트 경로에 대한 Lua 함수 테이블을 가져옵니다.
     *        스크립트가 아직 로드되지 않았다면 로드하고 캐시합니다.
     *        Lua 스크립트가 자신의 함수들을 담은 테이블을 반환한다고 가정합니다.
     * @param ScriptPath Lua 스크립트 파일 경로.
     * @return 스크립트의 함수들을 담은 sol::table. 로딩 실패 시 유효하지 않은 테이블 반환.
     */
    sol::table GetOrLoadScriptTable(const FString& ScriptPath);

    // --- Lua 상태 접근 ---
    // 내부 Lua 상태에 대한 참조를 가져옵니다.
    sol::state& GetLuaState();
    const FString& GetScriptsBasePath() const { return ScriptsBasePath; }
    const FString& GetTemplateLuaPath() const { return TemplateLuaPath; }

private:
    // --- 싱글톤을 위한 private 생성자 ---
    FLuaManager() = default; // 기본 생성자 사용

    // --- 핵심 멤버 ---
    sol::state LuaState; // Lua 인터프리터 상태
    std::unordered_map<FString, sol::table> LoadedScriptTables; // 로드된 스크립트 함수 테이블 캐시

    FString ScriptsBasePath = "Assets/Scripts/"; // 예시 경로
    FString TemplateLuaPath = "Assets/Scripts/DefaultActor.lua"; // 템플릿 파일 경로 (하드코딩 또는 설정 파일에서 로드)

    // --- 헬퍼 함수 ---
    // 공통 C++ 타입 및 함수를 Lua에 바인딩합니다.
    void BindCoreTypes();
    //전방 선언하듯이 모든 구조체를 미리 등록한다.
    void BindForwardDeclarations(sol::table& lua);
    void BindFStringToLua(sol::table& lua) ;
    
    static std::string LuaObjToString(const sol::object& obj, int depth = 0, bool showHidden = 0);
    bool bInitialized = false;

    // 싱글톤을 위한 정적 인스턴스
    static std::unique_ptr<FLuaManager> Instance;
    static std::once_flag InitInstanceFlag; // 스레드 안전 초기화를 위함
};
