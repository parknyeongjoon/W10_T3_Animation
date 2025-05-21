#include "Guid.h"
#include "Serialization/Archive.h"

void FGuid::Serialize(FArchive& Ar) const
{
    {
        // FArchive의 기본 타입 연산자를 사용하여 멤버 저장
        Ar << A << B << C << D;
    }
}

void FGuid::Deserialize(FArchive& Ar)
{
    {
        // FArchive의 기본 타입 연산자를 사용하여 멤버 로드
        Ar >> A >> B >> C >> D;
    }
}
