#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

class UGameManager : public UObject
{
    DECLARE_CLASS(UGameManager, UObject)
    
public:

    static UGameManager* Get();

    
    // 싱글톤을 위한 정적 인스턴스
    static std::unique_ptr<UGameManager> Instance;
};
