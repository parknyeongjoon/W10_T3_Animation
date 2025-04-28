#include "UGameManager.h"

UGameManager* UGameManager::Get()
{
    if (!Instance)
    {
        Instance = std::make_unique<UGameManager>();
    }
    return Instance.get();
}