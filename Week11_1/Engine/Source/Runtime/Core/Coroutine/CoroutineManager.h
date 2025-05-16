#pragma once

#include "Container/Array.h"
#include "IEnumerator.h"

class FCoroutineManager
{
public:
    ~FCoroutineManager();

    void StartCoroutine(IEnumerator* Coroutine);
    void Tick(float DeltaTime);
    void CleanupCoroutines();

    static float CurrentDeltaTime;
private:
    TArray<IEnumerator*> Coroutines;
};