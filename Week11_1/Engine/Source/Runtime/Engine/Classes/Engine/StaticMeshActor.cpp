#include "StaticMeshActor.h"

#include "Components/PrimitiveComponents/MeshComponents/StaticMeshComponents/StaticMeshComponent.h"


AStaticMeshActor::AStaticMeshActor()
{
    StaticMeshComponent = AddComponent<UStaticMeshComponent>(EComponentOrigin::Constructor);
    RootComponent = StaticMeshComponent;
}

