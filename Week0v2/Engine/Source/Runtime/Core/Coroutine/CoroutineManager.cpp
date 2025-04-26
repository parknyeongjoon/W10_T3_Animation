#include "CoroutineManager.h"
#include <algorithm>

void FCoroutineManager::StartCoroutine(IEnumerator* Coroutine)
{
    Coroutines.Add(Coroutine);
}

void FCoroutineManager::Tick(float DeltaTime)
{
    float deltaSeconds = DeltaTime * 1000.0f;

    for (auto*& Coroutine : Coroutines)
    {
        if (Coroutine)
        {
            if (!Coroutine->MoveNext(DeltaTime))
            {
                delete Coroutine;
                Coroutine = nullptr;
            }
        }
    }
}

void FCoroutineManager::CleanupCoroutines()
{
    Coroutines.Remove(nullptr);
}

FCoroutineManager::~FCoroutineManager()
{
    for (auto* Coroutine : Coroutines)
    {
        delete Coroutine;
    }
    Coroutines.Empty();
}
