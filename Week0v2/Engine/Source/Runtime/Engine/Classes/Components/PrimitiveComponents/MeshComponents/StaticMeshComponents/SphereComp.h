#pragma once
#include "StaticMeshComponent.h"

struct FSphereComponentInfo : public FStaticMeshComponentInfo
{
    DECLARE_ACTORCOMPONENT_INFO(FSphereComponentInfo);
    FSphereComponentInfo()
        : FStaticMeshComponentInfo()
    {
        InfoType = TEXT("FSphereComponentInfo");
        ComponentType = TEXT("USphereComp");
    }
    virtual void Copy(FActorComponentInfo& Other) override
    {
        FStaticMeshComponentInfo::Copy(Other);
    }
};

class USphereComp : public UStaticMeshComponent
{
    DECLARE_CLASS(USphereComp, UStaticMeshComponent)

public:
    USphereComp();
    USphereComp(const USphereComp&);
    virtual ~USphereComp() override;

    virtual void InitializeComponent() override;
    virtual void TickComponent(float DeltaTime) override;
   

};
