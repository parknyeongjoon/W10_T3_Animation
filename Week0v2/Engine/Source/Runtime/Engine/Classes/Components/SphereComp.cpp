#include "SphereComp.h"

#include "Engine/FLoaderOBJ.h"
#include "Engine/Source/Runtime/Core/Math/JungleMath.h"
#include "Engine/World.h"
#include "Engine/Source/Editor/PropertyEditor/ShowFlags.h"
#include "UnrealEd/EditorViewportClient.h"
#include "LevelEditor/SLevelEditor.h"
#include "UnrealEd/PrimitiveBatch.h"


USphereComp::USphereComp()
{
    AABB.max = {1, 1, 1};
    AABB.min = {-1, -1, -1};
}

USphereComp::USphereComp(const USphereComp& Other): Super(Other)
{
}

USphereComp::~USphereComp()
{
}

void USphereComp::InitializeComponent()
{
    Super::InitializeComponent();
    FManagerOBJ::CreateStaticMesh("Assets/Sphere.obj");
    SetStaticMesh(FManagerOBJ::GetStaticMesh(L"Sphere.obj"));
}

void USphereComp::TickComponent(float DeltaTime)
{
    Super::TickComponent(DeltaTime);
}