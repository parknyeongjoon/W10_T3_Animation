#include "InputCoreTypes.h"

FInputKeyManager& FInputKeyManager::Get()
{
    static FInputKeyManager Instance;
    return Instance;
}
