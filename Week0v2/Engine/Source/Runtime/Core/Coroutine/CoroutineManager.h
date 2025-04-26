#pragma once

#include <vector>
#include "IEnumerator.h"

class FCoroutineManager
{
public:
    ~FCoroutineManager();

    void StartCoroutine(IEnumerator* Coroutine);
    void Tick(float DeltaTime);
    void CleanupCoroutines();

private:
    std::vector<IEnumerator*> Coroutines;
};