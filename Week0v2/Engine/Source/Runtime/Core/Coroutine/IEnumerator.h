#pragma once
#pragma once

class IWaitObject;

class IEnumerator
{
public:
    virtual ~IEnumerator() = default;
    virtual bool MoveNext(float DeltaTime) = 0;
    virtual IWaitObject* GetCurrent() const = 0;
};
