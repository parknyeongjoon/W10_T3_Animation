#pragma once

class IWaitObject
{
public:
    virtual ~IWaitObject() = default;
    virtual bool IsReady(float DeltaTime) = 0;
};
